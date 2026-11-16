#include "GraphicsDevice.hpp"

#define VOLK_IMPLEMENTATION

#include <algorithm>
#include <cstring>
#include <SDL3/SDL_vulkan.h>
#include <volk.h>
#include "Core/Logger.hpp"
#include "Core/PlatformDefines.hpp"
#include "Gfx/Vulkan/VulkanGraphicsHelpers.hpp"
#include "Gfx/Vulkan/VulkanGraphicsTypes.hpp"
#include "Gfx/Vulkan/VulkanObjectFactory.hpp"
#include "Gfx/Vulkan/VulkanRenderContext.hpp"
#include "Gfx/Vulkan/VulkanTransferContext.hpp"
#include "Gfx/Vulkan/VulkanSwapchain.hpp"

namespace Bonsai
{
    /// @brief Vulkan debug callback.
    /// @param messageSeverity 
    /// @param messageType 
    /// @param callbackData 
    /// @param userData 
    /// @return 
    [[maybe_unused]]
    static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
        [[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
        VkDebugUtilsMessengerCallbackDataEXT const* callbackData,
        [[maybe_unused]] void* userData
    )
    {
        SPDLOG_TRACE("Vulkan: {}", callbackData->pMessage);
        return VK_FALSE;
    }

    /// @brief Check if a layer is available.
    /// @param availableLayers List of available layers.
    /// @param targetLayer Target layer name.
    /// @return A boolean indicating layer availability.
    [[nodiscard]]
    [[maybe_unused]]
    static bool IsLayerAvailable(std::vector<VkLayerProperties> const& availableLayers, char const* targetLayer)
    {
        for (auto const& layer : availableLayers)
        {
            if (std::strcmp(layer.layerName, targetLayer) == 0)
            {
                return true;
            }
        }

        return false;
    }

    /// @brief Check if an extension is available.
    /// @param availableExtensions List of available extensions.
    /// @param targetExtension Target extension name.
    /// @return A boolean indicating extension availability.
    [[nodiscard]]
    [[maybe_unused]]
    static bool IsExtensionAvailable(std::vector<VkExtensionProperties> const& availableExtensions, char const* targetExtension)
    {
        for (auto const& extension : availableExtensions)
        {
            if (std::strcmp(extension.extensionName, targetExtension) == 0)
            {
                return true;
            }
        }

        return false;
    }

    /// @brief Attempt to find a queue family index for a given set of required and ignored queue flags.
    /// @param queueFamilies Available queue families.
    /// @param requiredFlags Required queue flags.
    /// @param ignoredFlags Ignored queue flags. Optionally unused if a queue is not found for these flags.
    /// @return The found queue family, or VK_QUEUE_FAMILY_IGNORED on failure.
    [[nodiscard]]
    static uint32_t FindQueueFamily(
        VkInstance instance,
        VkPhysicalDevice physicalDevice,
        std::vector<VkQueueFamilyProperties> const& availableQueueFamilies,
        VkQueueFlagBits requiredFlags,
        VkQueueFlags ignoredFlags,
        bool requirePresentSupport
    )
    {
        // Perform first search with ignored flags for distinct queues
        for (uint32_t i = 0; i < availableQueueFamilies.size(); i++)
        {
            VkQueueFamilyProperties const& queueProperties = availableQueueFamilies[i];
            bool const hasPresentSupport = SDL_Vulkan_GetPresentationSupport(instance, physicalDevice, i);
            if ((queueProperties.queueFlags & requiredFlags) == requiredFlags
                && (queueProperties.queueFlags & ignoredFlags) == 0
                && (!requirePresentSupport || hasPresentSupport)
            ) {
                return i;
            }
        }

        // Perform fallback search without ignored flags for overlapping queue families
        for (uint32_t i = 0; i < availableQueueFamilies.size(); i++)
        {
            VkQueueFamilyProperties const& queueProperties = availableQueueFamilies[i];
            bool const hasPresentSupport = SDL_Vulkan_GetPresentationSupport(instance, physicalDevice, i);
            if ((queueProperties.queueFlags & requiredFlags) == requiredFlags    
                && (!requirePresentSupport || hasPresentSupport)
            ) {
                return i;
            }
        }

        return VK_QUEUE_FAMILY_IGNORED;
    }

    GraphicsDevice::GraphicsDevice(
        VulkanInstanceState const& instanceState,
        VulkanPhysicalDeviceInfo const& physicalDeviceInfo,
        VulkanDeviceState const& deviceState,
        std::unique_ptr<VulkanSharedDeviceState>&& sharedDeviceState,
        std::unique_ptr<VulkanRenderContext>&& renderContext,
        std::unique_ptr<VulkanTransferContext>&& transferContext,
        std::unique_ptr<VulkanObjectFactory>&& objectFactory
    )
        :
        _instanceState(instanceState),
        _physicalDeviceInfo(physicalDeviceInfo),
        _deviceState(deviceState),
        _sharedDeviceState(std::move(sharedDeviceState)),
        _renderContext(std::move(renderContext)),
        _transferContext(std::move(transferContext)),
        _objectFactory(std::move(objectFactory))
    {
        //
    }

    GraphicsDevice::~GraphicsDevice()
    {
        // Wait for all frames to finish executing
        _renderContext->waitForAllFrames();

        // Clean up Vulkan state
        _objectFactory.reset();
        _transferContext.reset();
        _renderContext.reset();
        _sharedDeviceState.reset();
        DestroyDeviceState(_deviceState);
        DestroyInstanceState(_instanceState);
        volkFinalize();
    }

    GraphicsDevice* GraphicsDevice::TryCreate()
    {
        // Load Vulkan symbols
        if (VK_CHECK_FAILED(volkInitialize())) {
            return nullptr;
        }

        if (!SDL_Vulkan_LoadLibrary(nullptr)) {
            return nullptr;
        }

        // Create instance state
        SPDLOG_TRACE("Creating Vulkan instance state");
        std::optional<VulkanInstanceState> const instanceState = CreateInstanceState();
        if (!instanceState.has_value())
        {
            SPDLOG_ERROR("Failed to create Vulkan instance state");
            return nullptr;
        }

        // Find a physical device
        SPDLOG_TRACE("Searching for Vulkan physical device");
        std::optional<VulkanPhysicalDeviceInfo> const physicalDeviceInfo = FindPhysicalDevice(instanceState.value());
        if (!physicalDeviceInfo.has_value())
        {
            SPDLOG_ERROR("Failed to find Vulkan physical device");
            DestroyInstanceState(instanceState.value());
            return nullptr;
        }

        // Create device state
        SPDLOG_TRACE("Creating Vulkan device state");
        std::optional<VulkanDeviceState> deviceState = CreateDeviceState(physicalDeviceInfo.value());
        if (!deviceState.has_value())
        {
            SPDLOG_ERROR("Failed to create Vulkan device state");
            DestroyInstanceState(instanceState.value());
            return nullptr;
        }

        // Create shared device state
        auto sharedDeviceState = std::make_unique<VulkanSharedDeviceState>();
        sharedDeviceState->framesInFlight = FramesInFlight;
        sharedDeviceState->physicalDeviceMemoryProperties2 = physicalDeviceInfo->physicalDeviceMemoryProperties2;
        sharedDeviceState->graphicsQueueFamily = physicalDeviceInfo->graphicsQueueFamily;
        sharedDeviceState->presentQueueFamily = physicalDeviceInfo->presentQueueFamily;
        sharedDeviceState->computeQueueFamily = physicalDeviceInfo->computeQueueFamily;
        sharedDeviceState->transferQueueFamily = physicalDeviceInfo->transferQueueFamily;
        sharedDeviceState->graphicsQueue = deviceState->graphicsQueue;
        sharedDeviceState->presentQueue = deviceState->presentQueue;
        sharedDeviceState->computeQueue = deviceState->computeQueue;
        sharedDeviceState->transferQueue = deviceState->transferQueue;

        // Create Vulkan render context
        auto renderContext = std::unique_ptr<VulkanRenderContext>(VulkanRenderContext::TryCreate(deviceState->device, sharedDeviceState.get()));
        if (!renderContext)
        {
            SPDLOG_ERROR("Failed to create Vulkan render context");
            DestroyDeviceState(deviceState.value());
            DestroyInstanceState(instanceState.value());
            return nullptr;
        }

        // Create Vulkan transfer context
        auto transferContext = std::unique_ptr<VulkanTransferContext>(VulkanTransferContext::TryCreate(deviceState->device, sharedDeviceState.get()));
        if (!transferContext)
        {
            SPDLOG_ERROR("Failed to create Vulkan transfer context");
            DestroyDeviceState(deviceState.value());
            DestroyInstanceState(instanceState.value());
            return nullptr;
        }

        // Create Vulkan object factory
        auto objectFactory = std::unique_ptr<VulkanObjectFactory>(VulkanObjectFactory::TryCreate(instanceState->instance, physicalDeviceInfo->physicalDevice, deviceState->device, sharedDeviceState.get()));
        if (!objectFactory)
        {
            SPDLOG_ERROR("Failed to create Vulkan render context");
            renderContext.reset();
            DestroyDeviceState(deviceState.value());
            DestroyInstanceState(instanceState.value());
            return nullptr;
        }

        return new GraphicsDevice(
            instanceState.value(),
            physicalDeviceInfo.value(),
            deviceState.value(),
            std::move(sharedDeviceState),
            std::move(renderContext),
            std::move(transferContext),
            std::move(objectFactory)
        );
    }

    GraphicsBuffer* GraphicsDevice::CreateBuffer(GraphicsBufferDesc const& bufferDesc)
    {
        return _objectFactory->CreateBuffer(bufferDesc);
    }

    GraphicsTexture* GraphicsDevice::CreateTexture(GraphicsTextureDesc const& textureDesc)
    {
        return _objectFactory->CreateTexture(textureDesc);
    }

    GraphicsTextureView* GraphicsDevice::CreateTextureView(GraphicsTextureViewDesc const& textureViewDesc)
    {
        return _objectFactory->CreateTextureView(textureViewDesc);
    }

    GraphicsSampler* GraphicsDevice::CreateSampler(GraphicsSamplerDesc const& samplerDesc)
    {
        return _objectFactory->CreateSampler(samplerDesc);
    }

    GraphicsDescriptorSetLayout* GraphicsDevice::CreateDescriptorSetLayout(GraphicsDescriptorSetLayoutDesc const& descriptorSetLayoutDesc)
    {
        return _objectFactory->CreateDescriptorSetLayout(descriptorSetLayoutDesc);
    }

    GraphicsPipelineLayout* GraphicsDevice::CreatePipelineLayout(GraphicsPipelineLayoutDesc const& pipelineLayoutDesc)
    {
        return _objectFactory->CreatePipelineLayout(pipelineLayoutDesc);
    }

    GraphicsRenderPipeline* GraphicsDevice::CreateRenderPipeline(GraphicsRenderPipelineDesc const& renderPipelineDesc)
    {
        return _objectFactory->CreateRenderPipeline(renderPipelineDesc);
    }

    GraphicsComputePipeline* GraphicsDevice::CreateComputePipeline(GraphicsComputePipelineDesc const& computePipelineDesc)
    {
        return _objectFactory->CreateComputePipeline(computePipelineDesc);
    }

    GraphicsSwapchain* GraphicsDevice::CreateSwapchain(GraphicsSwapchainDesc const& swapchainDesc)
    {
        return _objectFactory->CreateSwapchain(swapchainDesc);
    }

    void GraphicsDevice::DestroyBuffer(GraphicsBuffer* buffer)
    {
        if (buffer != nullptr) {
            buffer->ReleaseRef();
        }
    }

    void GraphicsDevice::DestroyTexture(GraphicsTexture* texture)
    {
        if (texture != nullptr) {
            texture->ReleaseRef();
        }
    }
    
    void GraphicsDevice::DestroyTextureView(GraphicsTextureView* textureView)
    {
        if (textureView != nullptr) {
            textureView->ReleaseRef();
        }
    }

    void GraphicsDevice::DestroySampler(GraphicsSampler* sampler)
    {
        if (sampler != nullptr) {
            sampler->ReleaseRef();
        }
    }

    void GraphicsDevice::DestroyPipelineLayout(GraphicsPipelineLayout* pipelineLayout)
    {
        if (pipelineLayout != nullptr) {
            pipelineLayout->ReleaseRef();
        }
    }

    void GraphicsDevice::DestroyRenderPipeline(GraphicsRenderPipeline* renderPipeline)
    {
        if (renderPipeline != nullptr) {
            renderPipeline->ReleaseRef();
        }
    }

    void GraphicsDevice::DestroyComputePipeline(GraphicsComputePipeline* computePipeline)
    {
        if (computePipeline != nullptr) {
            computePipeline->ReleaseRef();
        }
    }

    void GraphicsDevice::DestroySwapchain(GraphicsSwapchain* swapchain)
    {
        if (swapchain != nullptr) {
            swapchain->ReleaseRef();
        }
    }

    void GraphicsDevice::CopyToBuffer(GraphicsBuffer* buffer, void const* data, size_t size, size_t offset)
    {
        // Get mapped buffer pointer
        void* bufferData = buffer->GetMappedMemory();
        if (bufferData == nullptr) {
            return;
        }

        // Copy data to buffer
        bufferData = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(bufferData) + offset);
        std::memcpy(bufferData, data, size);
    }

    void GraphicsDevice::CopyFromBuffer(GraphicsBuffer* buffer, void* outData, size_t size, size_t offset)
    {
        // Check output pointer
        if (outData == nullptr) {
            return;
        }

        // Get mapped buffer pointer
        void* bufferData = buffer->GetMappedMemory();
        if (bufferData == nullptr) {
            return;
        }

        // Copy data from buffer
        bufferData = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(bufferData) + offset);
        std::memcpy(outData, bufferData, size);
    }

    FrameContext* GraphicsDevice::StartFrame(GraphicsSwapchain** swapchains, size_t swapchainCount)
    {
        return _renderContext->StartFrame(swapchains, swapchainCount);
    }

    GraphicsCommandBuffer* GraphicsDevice::BeginGraphicsCommandBuffer(FrameContext* frameContext)
    {
        return frameContext->BeginNextGraphicsCommandBuffer();
    }

    void GraphicsDevice::SubmitFrame(FrameContext* frameContext)
    {
        auto* vulkanFrameContext = static_cast<VulkanFrameContext*>(frameContext);
        _renderContext->RecordFrame(vulkanFrameContext);
        _renderContext->SubmitFrame(vulkanFrameContext);
    }

    TransferBatch* GraphicsDevice::StartTransferBatch()
    {
        return _transferContext->StartTransferBatch();
    }

    GraphicsCommandBuffer* GraphicsDevice::BeginTransferCommandBuffer(TransferBatch* transferBatch)
    {
        return transferBatch->BeginNextTransferCommandBuffer();
    }

    void GraphicsDevice::SubmitTransferBatch(TransferBatch* transferBatch)
    {
        auto* vulkanTransferBatch = static_cast<VulkanTransferBatch*>(transferBatch);
        _transferContext->RecordTransferBatch(vulkanTransferBatch);
        _transferContext->SubmitTransferBatch(vulkanTransferBatch);
    }

    void GraphicsDevice::WaitForTransferBatch(TransferBatch* transferBatch)
    {
        auto* vulkanTransferBatch = static_cast<VulkanTransferBatch*>(transferBatch);
        _transferContext->WaitForTransferBatch(vulkanTransferBatch);
    }

    GraphicsTexture* GraphicsDevice::GetSwapchainBackbuffer(GraphicsSwapchain* swapchain)
    {
        if (swapchain == nullptr) {
            return nullptr;
        }

        // Return the backbuffer texture
        GraphicsTexture* backbuffer = swapchain->GetBackbuffer();
        backbuffer->AddRef();
        return backbuffer;
    }

    GraphicsTextureView* GraphicsDevice::GetSwapchainBackbufferView(GraphicsSwapchain* swapchain)
    {
        if (swapchain == nullptr) {
            return nullptr;
        }

        // Return the backbuffer view
        GraphicsTextureView* backbufferView = swapchain->GetBackbufferView();
        backbufferView->AddRef();
        return backbufferView;
    }

    Extent2D GraphicsDevice::GetSwapchainExtent(GraphicsSwapchain* swapchain)
    {
        if (swapchain == nullptr) {
            return { 0, 0 };
        }

        return swapchain->GetExtent();
    }

    void GraphicsDevice::ResizeSwapchain(GraphicsSwapchain* swapchain, uint32_t width, uint32_t height)
    {
        auto* vulkanSwapchain = static_cast<VulkanSwapchain*>(swapchain);
        if (vulkanSwapchain == nullptr) {
            return;
        }

        if (!vulkanSwapchain->Resize(width, height)) {
            SPDLOG_WARN("Failed to resize swap chain to {} x {}", width, height);
        }
    }

    void GraphicsDevice::WaitForIdle()
    {
        // Wait for all frames to finish rendering
        _renderContext->waitForAllFrames();
    }

    std::optional<GraphicsDevice::VulkanInstanceState> GraphicsDevice::CreateInstanceState()
    {
        // Query available instance layers and extensions
        std::vector<VkLayerProperties> const availableInstanceLayers = []() {
            uint32_t layerCount = 0;
            vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
            std::vector<VkLayerProperties> layers(layerCount);
            vkEnumerateInstanceLayerProperties(&layerCount, layers.data());
            return layers;
        }();

        std::vector<VkExtensionProperties> const availableInstanceExtensions = []() {
            uint32_t extensionCount = 0;
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
            std::vector<VkExtensionProperties> extensions(extensionCount);
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
            return extensions;
        }();

        // Query required window system extensions
        std::vector<char const*> const windowSystemExtensions = []() {
            uint32_t windowSystemExtensionCount = 0;
            char const* const* windowSystemExtensions = SDL_Vulkan_GetInstanceExtensions(&windowSystemExtensionCount);
            return std::vector<char const*>(windowSystemExtensions, windowSystemExtensions + windowSystemExtensionCount);
        }();

        // Set up enabled layers and extensions
        std::vector<char const*> enabledLayerNames;
        std::vector<char const*> enabledExtensionNames;
        enabledExtensionNames.insert(
            enabledExtensionNames.end(),
            windowSystemExtensions.begin(),
            windowSystemExtensions.end()
        );

        // Enable debug layers and extensions
        if constexpr (BONSAI_BUILD_TYPE_DEBUG)
        {
            // Enable optional validation layer
            if (IsLayerAvailable(availableInstanceLayers, "VK_LAYER_KHRONOS_validation")) {
                enabledLayerNames.push_back("VK_LAYER_KHRONOS_validation");
            }

            // Enable optional synchronization layer
            if (IsLayerAvailable(availableInstanceLayers, "VK_LAYER_KHRONOS_synchronization2")) {
                enabledLayerNames.push_back("VK_LAYER_KHRONOS_synchronization2");
            }

            // Enable debug utils extension
            if (IsExtensionAvailable(availableInstanceExtensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
            {
                SPDLOG_ERROR("Enabled required instance extension: {}", VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
                enabledExtensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            }
            else
            {
                SPDLOG_ERROR("Failed to enable required instance extension: {}", VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
                return {};
            }
        }

        // Create instance
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pNext = nullptr;
        appInfo.pApplicationName = "Bonsai Engine";
        appInfo.applicationVersion = 0;
        appInfo.pEngineName = "Bonsai Engine";
        appInfo.engineVersion = 0;
        appInfo.apiVersion = ReferenceVulkanApiVersion;

        VkInstanceCreateInfo instanceCreateInfo{};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pNext = nullptr;
        instanceCreateInfo.flags = 0;
        instanceCreateInfo.pApplicationInfo = &appInfo;
        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(enabledLayerNames.size());
        instanceCreateInfo.ppEnabledLayerNames = enabledLayerNames.data();
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensionNames.size());
        instanceCreateInfo.ppEnabledExtensionNames = enabledExtensionNames.data();

#if     BONSAI_BUILD_TYPE_DEBUG
        VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};
        debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugMessengerCreateInfo.pNext = nullptr;
        debugMessengerCreateInfo.flags = 0;
        debugMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugMessengerCreateInfo.pfnUserCallback = VulkanDebugCallback;
        debugMessengerCreateInfo.pUserData = nullptr;
        instanceCreateInfo.pNext = &debugMessengerCreateInfo;
#endif  //BONSAI_BUILD_TYPE_DEBUG

        VkInstance instance = VK_NULL_HANDLE;
        if (VK_CHECK_FAILED(vkCreateInstance(&instanceCreateInfo, nullptr, &instance)))
        {
            SPDLOG_ERROR("Failed to create Vulkan instance");
            return {};
        }
        
        // Load function pointers at instance level
        volkLoadInstance(instance);

        // Create debug messenger
        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
#if     BONSAI_BUILD_TYPE_DEBUG
        if (VK_CHECK_FAILED(vkCreateDebugUtilsMessengerEXT(instance, &debugMessengerCreateInfo, nullptr, &debugMessenger)))
        {
            SPDLOG_ERROR("Failed to create Vulkan debug messenger");
            vkDestroyInstance(instance, nullptr);
            return {};
        }
#endif  //BONSAI_BUILD_TYPE_DEBUG

        return VulkanInstanceState {
            instance,
            debugMessenger,
        };
    }

    std::optional<GraphicsDevice::VulkanPhysicalDeviceInfo> GraphicsDevice::FindPhysicalDevice(VulkanInstanceState const& instanceState)
    {
        // Query available physical devices
        std::vector<VkPhysicalDevice> const physicalDevices = [&instanceState]() {
            uint32_t deviceCount = 0;
            vkEnumeratePhysicalDevices(instanceState.instance, &deviceCount, nullptr);
            std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
            vkEnumeratePhysicalDevices(instanceState.instance, &deviceCount, physicalDevices.data());
            return physicalDevices;
        }();

        // Set up physical device info structures
        size_t const physicalDeviceCount = physicalDevices.size();
        std::vector<VulkanPhysicalDeviceInfo> physicalDeviceInfos(physicalDeviceCount);
        for (size_t i = 0; i < physicalDeviceCount; i++)
        {
            VulkanPhysicalDeviceInfo& physicalDeviceInfo = physicalDeviceInfos[i];
            physicalDeviceInfo.physicalDevice = physicalDevices[i];
            physicalDeviceInfo.isSupported = false;

            // Get physical device properties
            physicalDeviceInfo.physicalDeviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
            physicalDeviceInfo.physicalDeviceProperties2.pNext = nullptr;

            vkGetPhysicalDeviceProperties2(physicalDeviceInfo.physicalDevice, &physicalDeviceInfo.physicalDeviceProperties2);

            // Get physical device memory properties
            physicalDeviceInfo.physicalDeviceMemoryProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
            physicalDeviceInfo.physicalDeviceMemoryProperties2.pNext = nullptr;

            vkGetPhysicalDeviceMemoryProperties2(physicalDeviceInfo.physicalDevice, &physicalDeviceInfo.physicalDeviceMemoryProperties2);

            // Get physical device features
            physicalDeviceInfo.physicalDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            physicalDeviceInfo.physicalDeviceVulkan11Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
            physicalDeviceInfo.physicalDeviceVulkan12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
            physicalDeviceInfo.physicalDeviceVulkan13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;

            physicalDeviceInfo.physicalDeviceFeatures2.pNext = &physicalDeviceInfo.physicalDeviceVulkan11Features;
            physicalDeviceInfo.physicalDeviceVulkan11Features.pNext = &physicalDeviceInfo.physicalDeviceVulkan12Features;
            physicalDeviceInfo.physicalDeviceVulkan12Features.pNext = &physicalDeviceInfo.physicalDeviceVulkan13Features;
            physicalDeviceInfo.physicalDeviceVulkan13Features.pNext = nullptr;
            
            vkGetPhysicalDeviceFeatures2(physicalDeviceInfo.physicalDevice, &physicalDeviceInfo.physicalDeviceFeatures2);

            // Query queue family properties
            std::vector<VkQueueFamilyProperties> const queueFamilyProperties = [&physicalDeviceInfo]() {
                uint32_t queueCount = 0;
                vkGetPhysicalDeviceQueueFamilyProperties(physicalDeviceInfo.physicalDevice, &queueCount, nullptr);
                std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueCount);
                vkGetPhysicalDeviceQueueFamilyProperties(physicalDeviceInfo.physicalDevice, &queueCount, queueFamilyProperties.data());
                return queueFamilyProperties;
            }();

            // Find queue families for physical device
            physicalDeviceInfo.graphicsQueueFamily = FindQueueFamily(instanceState.instance, physicalDeviceInfo.physicalDevice, queueFamilyProperties, VK_QUEUE_GRAPHICS_BIT, 0, false);
            physicalDeviceInfo.presentQueueFamily = FindQueueFamily(instanceState.instance, physicalDeviceInfo.physicalDevice, queueFamilyProperties, VK_QUEUE_GRAPHICS_BIT, 0, true);
            physicalDeviceInfo.computeQueueFamily = FindQueueFamily(instanceState.instance, physicalDeviceInfo.physicalDevice, queueFamilyProperties, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_GRAPHICS_BIT, false);
            physicalDeviceInfo.transferQueueFamily = FindQueueFamily(instanceState.instance, physicalDeviceInfo.physicalDevice, queueFamilyProperties, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT, false);
        }

        // Verify physical device support
        for (auto& physicalDeviceInfo : physicalDeviceInfos)
        {
            // Assume the device is supported
            physicalDeviceInfo.isSupported = true;

            // Check if the device API version is above the minimal supported version
            if (physicalDeviceInfo.physicalDeviceProperties2.properties.apiVersion < ReferenceVulkanApiVersion)
            {
                SPDLOG_WARN("Physical device was under minimum required Vulkan version: \"{}\" ({})",
                    physicalDeviceInfo.physicalDeviceProperties2.properties.deviceName,
                    physicalDeviceInfo.physicalDeviceProperties2.properties.deviceID
                );
                physicalDeviceInfo.isSupported = false;
            }

            // Check if all required features are supported
            if (physicalDeviceInfo.physicalDeviceVulkan13Features.dynamicRendering != VK_TRUE)
            {
                SPDLOG_WARN("Physical device does not support the dynamic rendering feature: \"{}\" ({})",
                    physicalDeviceInfo.physicalDeviceProperties2.properties.deviceName,
                    physicalDeviceInfo.physicalDeviceProperties2.properties.deviceID
                );
                physicalDeviceInfo.isSupported = false;
            }

            if (physicalDeviceInfo.physicalDeviceVulkan13Features.synchronization2 != VK_TRUE)
            {
                SPDLOG_WARN("Physical device does not support the synchronization2 feature: \"{}\" ({})",
                    physicalDeviceInfo.physicalDeviceProperties2.properties.deviceName,
                    physicalDeviceInfo.physicalDeviceProperties2.properties.deviceID
                );
                physicalDeviceInfo.isSupported = false;
            }

            // Check if the required queue families were found
            if (physicalDeviceInfo.graphicsQueueFamily == VK_QUEUE_FAMILY_IGNORED
                || physicalDeviceInfo.presentQueueFamily == VK_QUEUE_FAMILY_IGNORED
                || physicalDeviceInfo.computeQueueFamily == VK_QUEUE_FAMILY_IGNORED
                || physicalDeviceInfo.transferQueueFamily == VK_QUEUE_FAMILY_IGNORED)
            {
                SPDLOG_WARN("Physical device does not support the required queue families: \"{}\" ({})",
                    physicalDeviceInfo.physicalDeviceProperties2.properties.deviceName,
                    physicalDeviceInfo.physicalDeviceProperties2.properties.deviceID
                );
                physicalDeviceInfo.isSupported = false;
            }
        }

        // Select first supported physical device
        for (auto const& physicalDeviceInfo : physicalDeviceInfos)
        {
            if (physicalDeviceInfo.isSupported)
            {
                SPDLOG_TRACE("Selected physical device: \"{}\" ({})",
                    physicalDeviceInfo.physicalDeviceProperties2.properties.deviceName,
                    physicalDeviceInfo.physicalDeviceProperties2.properties.deviceID
                );
                return physicalDeviceInfo;
            }
        }

        // No supported device found
        return {};
    }

    std::optional<GraphicsDevice::VulkanDeviceState> GraphicsDevice::CreateDeviceState(VulkanPhysicalDeviceInfo const& physicalDeviceInfo)
    {
        // Query available device extensions
        std::vector<VkExtensionProperties> const availableDeviceExtensions = [&physicalDeviceInfo]() {
            uint32_t extensionCount = 0;
            vkEnumerateDeviceExtensionProperties(physicalDeviceInfo.physicalDevice, nullptr, &extensionCount, nullptr);
            std::vector<VkExtensionProperties> availableExtensions(extensionCount);
            vkEnumerateDeviceExtensionProperties(physicalDeviceInfo.physicalDevice, nullptr, &extensionCount, availableExtensions.data());
            return availableExtensions;
        }();

        // Set up enabled device extensions
        std::vector<char const*> enabledExtensionNames;
        if (IsExtensionAvailable(availableDeviceExtensions, VK_KHR_SWAPCHAIN_EXTENSION_NAME))
        {
            SPDLOG_TRACE("Enabled required device extension: {}", VK_KHR_SWAPCHAIN_EXTENSION_NAME);
            enabledExtensionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        }
        else
        {
            SPDLOG_ERROR("Failed to enable required device extension: {}", VK_KHR_SWAPCHAIN_EXTENSION_NAME);
            return {};
        }

        // Set up enabled device features
        VkPhysicalDeviceFeatures2 physicalDeviceFeatures2{};
        VkPhysicalDeviceVulkan11Features physicalDeviceVulkan11Features{};
        VkPhysicalDeviceVulkan12Features physicalDeviceVulkan12Features{};
        VkPhysicalDeviceVulkan13Features physicalDeviceVulkan13Features{};
        physicalDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        physicalDeviceVulkan11Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
        physicalDeviceVulkan12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        physicalDeviceVulkan13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;

        physicalDeviceFeatures2.pNext = &physicalDeviceVulkan11Features;
        physicalDeviceVulkan11Features.pNext = &physicalDeviceVulkan12Features;
        physicalDeviceVulkan12Features.pNext = &physicalDeviceVulkan13Features;
        physicalDeviceVulkan13Features.pNext = nullptr;

        if (physicalDeviceInfo.physicalDeviceVulkan13Features.dynamicRendering == VK_TRUE) {
            physicalDeviceVulkan13Features.dynamicRendering = VK_TRUE;
        }
        if (physicalDeviceInfo.physicalDeviceVulkan13Features.synchronization2 == VK_TRUE) {
            physicalDeviceVulkan13Features.synchronization2 = VK_TRUE;
        }

        // Set up device queue create infos
        std::vector<uint32_t> const uniqueQueueFamilyIndices = GetUniqueQueueFamilyIndices(physicalDeviceInfo);
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        queueCreateInfos.reserve(uniqueQueueFamilyIndices.size());

        static constexpr float DefaultQueuePriorities[] = { 1.0F };
        for (auto const& queueFamilyIdx : uniqueQueueFamilyIndices)
        {
            VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
            deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            deviceQueueCreateInfo.pNext = nullptr;
            deviceQueueCreateInfo.flags = 0;
            deviceQueueCreateInfo.queueFamilyIndex = queueFamilyIdx;
            deviceQueueCreateInfo.queueCount = 1;
            deviceQueueCreateInfo.pQueuePriorities = DefaultQueuePriorities;

            queueCreateInfos.push_back(deviceQueueCreateInfo);
        }

        // Create logical device
        VkDeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pNext = &physicalDeviceFeatures2;
        deviceCreateInfo.flags = 0;
        deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensionNames.size());
        deviceCreateInfo.ppEnabledExtensionNames = enabledExtensionNames.data();
        deviceCreateInfo.pEnabledFeatures = nullptr;

        VkDevice device = VK_NULL_HANDLE;
        if (VK_CHECK_FAILED(vkCreateDevice(physicalDeviceInfo.physicalDevice, &deviceCreateInfo, nullptr, &device)))
        {
            SPDLOG_ERROR("Failed to create Vulkan logical device");
            return {};
        }

        // Get device queues
        VkQueue graphicsQueue = VK_NULL_HANDLE;
        VkQueue presentQueue = VK_NULL_HANDLE;
        VkQueue computeQueue = VK_NULL_HANDLE;
        VkQueue transferQueue = VK_NULL_HANDLE;
        vkGetDeviceQueue(device, physicalDeviceInfo.graphicsQueueFamily, 0, &graphicsQueue);
        vkGetDeviceQueue(device, physicalDeviceInfo.presentQueueFamily, 0, &presentQueue);
        vkGetDeviceQueue(device, physicalDeviceInfo.computeQueueFamily, 0, &computeQueue);
        vkGetDeviceQueue(device, physicalDeviceInfo.transferQueueFamily, 0, &transferQueue);

        // Load vulkan pointers at device level
        volkLoadDevice(device);
        
        return VulkanDeviceState {
            device,
            graphicsQueue,
            presentQueue,
            computeQueue,
            transferQueue,
        };
    }

    void GraphicsDevice::DestroyInstanceState(VulkanInstanceState const& instanceState)
    {
#if     BONSAI_BUILD_TYPE_DEBUG
        vkDestroyDebugUtilsMessengerEXT(instanceState.instance, instanceState.debugMessenger, nullptr);
#endif  //BONSAI_BUILD_TYPE_DEBUG
        vkDestroyInstance(instanceState.instance, nullptr);
    }

    void GraphicsDevice::DestroyDeviceState(VulkanDeviceState const& deviceState)
    {
        vkDestroyDevice(deviceState.device, nullptr);
    }

    std::vector<uint32_t> GraphicsDevice::GetUniqueQueueFamilyIndices(VulkanPhysicalDeviceInfo const& physicalDeviceInfo)
    {
        // Get list of queue families
        std::vector<uint32_t> queueFamilies = {
            physicalDeviceInfo.graphicsQueueFamily,
            physicalDeviceInfo.presentQueueFamily,
            physicalDeviceInfo.computeQueueFamily,
            physicalDeviceInfo.transferQueueFamily,
        };

        // Remove duplicates from list
        std::sort(queueFamilies.begin(), queueFamilies.end());
        queueFamilies.erase(std::unique(queueFamilies.begin(), queueFamilies.end()), queueFamilies.end());

        return queueFamilies;
    }
} // namespace Bonsai
