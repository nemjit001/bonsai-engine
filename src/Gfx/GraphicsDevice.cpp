#include "GraphicsDevice.hpp"

#define VOLK_IMPLEMENTATION
#include <SDL3/SDL_vulkan.h>
#include <volk.h>
#include "Core/Logger.hpp"
#include "Core/PlatformDefines.hpp"

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

    GraphicsDevice::GraphicsDevice(VulkanInstanceState const& instanceState)
        :
        _instanceState(instanceState)
    {
        //
    }

    GraphicsDevice::~GraphicsDevice()
    {
        DestroyInstanceState(_instanceState);
        volkFinalize();
    }

    GraphicsDevice* GraphicsDevice::TryCreate()
    {
        // Load Vulkan symbols
        if (VK_CHECK_FAILED(volkInitialize())) {
            return nullptr;
        }

        // Create instance state
        VulkanInstanceState instanceState{};
        if (!CreateInstanceState(instanceState)) {
            return nullptr;
        }

        return new GraphicsDevice(instanceState);
    }

    bool GraphicsDevice::CreateInstanceState(VulkanInstanceState& instanceState)
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
            if (!IsExtensionAvailable(availableInstanceExtensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
                return false;
            }
            enabledExtensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
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

        if (VK_CHECK_FAILED(vkCreateInstance(&instanceCreateInfo, nullptr, &instanceState.instance))) {
            return false;
        }
        
        // Load function pointers at instance level
        volkLoadInstance(instanceState.instance);

#if     BONSAI_BUILD_TYPE_DEBUG
        // Create debug messenger
        if (VK_CHECK_FAILED(vkCreateDebugUtilsMessengerEXT(instanceState.instance, &debugMessengerCreateInfo, nullptr, &instanceState.debugMessenger)))
        {
            vkDestroyInstance(instanceState.instance, nullptr);
            return false;
        }
#endif  //BONSAI_BUILD_TYPE_DEBUG

        return true;
    }

    void GraphicsDevice::DestroyInstanceState(VulkanInstanceState const& instanceState)
    {
#if     BONSAI_BUILD_TYPE_DEBUG
        vkDestroyDebugUtilsMessengerEXT(instanceState.instance, instanceState.debugMessenger, nullptr);
#endif  //BONSAI_BUILD_TYPE_DEBUG
        vkDestroyInstance(instanceState.instance, nullptr);
    }
} // namespace Bonsai
