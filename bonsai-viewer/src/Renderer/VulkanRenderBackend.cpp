#include "VulkanRenderBackend.hpp"
#if BONSAI_VULKAN_SUPPORT

#define VMA_IMPLEMENTATION
#define VOLK_IMPLEMENTATION

#include <algorithm>
#include <array>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <spirv-reflect/spirv_reflect.h>
#include <volk.h>
#include <vk_mem_alloc.h>
#include "Logging.hpp"
#include "Renderer/RenderResourcePool.hpp"
#include "Renderer/ShaderCompiler.hpp"

#define BONSAI_VULKAN_API_VERSION                   VK_API_VERSION_1_3

#define BONSAI_VK_VERTEX_SHADER_TARGET_PROFILE      "vs_6_5"
#define BONSAI_VK_HULL_SHADER_TARGET_PROFILE        "hs_6_5"
#define BONSAI_VK_DOMAIN_SHADER_TARGET_PROFILE      "ds_6_5"
#define BONSAI_VK_GEOMETRY_SHADER_TARGET_PROFILE    "gs_6_5"
#define BONSAI_VK_PIXEL_SHADER_TARGET_PROFILE       "ps_6_5"
#define BONSAI_VK_COMPUTE_SHADER_TARGET_PROFILE     "cs_6_5"

#ifndef NDEBUG
static constexpr bool RendererDebugEnabled = true;
#else
static constexpr bool RendererDebugEnabled = false;
#endif //!NDEBUG

/// @brief Instance extension names that are enabled by default.
static constexpr std::array DefaultInstanceDebugExtensionNames = {
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
};

/// @brief Instance layer names that are enabled on debug.
static constexpr std::array DefaultInstanceDebugLayerNames = {
    "VK_LAYER_KHRONOS_validation",
};

/// @brief Device extension names that are enabled by default.
static constexpr std::array DefaultDeviceExtensionNames = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,
};

/// @brief Default shader compiler compilation args.
static constexpr std::array DefaultShaderCompilationArgs = {
    DXC_ARG_DEBUG,
    DXC_ARG_PACK_MATRIX_ROW_MAJOR,
    DXC_ARG_OPTIMIZATION_LEVEL3,
    DXC_ARG_GENERATE_SPIRV,
    DXC_ARG_SPIRV_PRESERVE_BINDINGS,
    DXC_ARG_SPIRV_TARGET_ENV("vulkan1.3"),
};

/// @brief Lookup table for GPUHeapType -> VkMemoryPropertyFlags
static constexpr std::array GPUHeapTypeLookupTable = {
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | 0,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
};
static_assert(GPUHeapTypeLookupTable.size() == GPUHeapType_COUNT);

/// @brief Lookup table for GPUFormat -> VkFormat conversion.
static constexpr std::array GPUFormatLookupTable = {
    VK_FORMAT_UNDEFINED,
    VK_FORMAT_R8_UNORM,
    VK_FORMAT_R8_SNORM,
    VK_FORMAT_R8_UINT,
    VK_FORMAT_R8_SINT,
    VK_FORMAT_R8G8_UNORM,
    VK_FORMAT_R8G8_SNORM,
    VK_FORMAT_R8G8_UINT,
    VK_FORMAT_R8G8_SINT,
    VK_FORMAT_B8G8R8_UNORM,
    VK_FORMAT_B8G8R8_SNORM,
    VK_FORMAT_B8G8R8_UINT,
    VK_FORMAT_B8G8R8_SINT,
    VK_FORMAT_B8G8R8_SRGB,
    VK_FORMAT_R8G8B8A8_UNORM,
    VK_FORMAT_R8G8B8A8_SNORM,
    VK_FORMAT_R8G8B8A8_UINT,
    VK_FORMAT_R8G8B8A8_SINT,
    VK_FORMAT_R8G8B8A8_SRGB,
    VK_FORMAT_B8G8R8A8_UNORM,
    VK_FORMAT_B8G8R8A8_SNORM,
    VK_FORMAT_B8G8R8A8_UINT,
    VK_FORMAT_B8G8R8A8_SINT,
    VK_FORMAT_B8G8R8A8_SRGB,
    VK_FORMAT_R16_UNORM,
    VK_FORMAT_R16_SNORM,
    VK_FORMAT_R16_UINT,
    VK_FORMAT_R16_SINT,
    VK_FORMAT_R16_SFLOAT,
    VK_FORMAT_R16G16_UNORM,
    VK_FORMAT_R16G16_SNORM,
    VK_FORMAT_R16G16_UINT,
    VK_FORMAT_R16G16_SINT,
    VK_FORMAT_R16G16_SFLOAT,
    VK_FORMAT_R16G16B16_UNORM,
    VK_FORMAT_R16G16B16_SNORM,
    VK_FORMAT_R16G16B16_UINT,
    VK_FORMAT_R16G16B16_SINT,
    VK_FORMAT_R16G16B16_SFLOAT,
    VK_FORMAT_R16G16B16A16_UNORM,
    VK_FORMAT_R16G16B16A16_SNORM,
    VK_FORMAT_R16G16B16A16_UINT,
    VK_FORMAT_R16G16B16A16_SINT,
    VK_FORMAT_R16G16B16A16_SFLOAT,
    VK_FORMAT_R32_UINT,
    VK_FORMAT_R32_SINT,
    VK_FORMAT_R32_SFLOAT,
    VK_FORMAT_R32G32_UINT,
    VK_FORMAT_R32G32_SINT,
    VK_FORMAT_R32G32_SFLOAT,
    VK_FORMAT_R32G32B32_UINT,
    VK_FORMAT_R32G32B32_SINT,
    VK_FORMAT_R32G32B32_SFLOAT,
    VK_FORMAT_R32G32B32A32_UINT,
    VK_FORMAT_R32G32B32A32_SINT,
    VK_FORMAT_R32G32B32A32_SFLOAT,
    VK_FORMAT_D16_UNORM,
    VK_FORMAT_D32_SFLOAT,
    VK_FORMAT_S8_UINT,
    VK_FORMAT_D16_UNORM_S8_UINT,
    VK_FORMAT_D24_UNORM_S8_UINT,
    VK_FORMAT_D32_SFLOAT_S8_UINT,
};
static_assert(GPUFormatLookupTable.size() == GPUFormat_COUNT);

/// @brief Lookup table for GPUTextureType -> VkImageType conversion.
static constexpr std::array GPUTextureTypeLookupTable = {
    VK_IMAGE_TYPE_1D,
    VK_IMAGE_TYPE_2D,
    VK_IMAGE_TYPE_3D,
};
static_assert(GPUTextureTypeLookupTable.size() == GPUTextureType_COUNT);

/// @brief The DeviceFeatures struct contains vulkan device features structs used during device selection.
struct DeviceFeatures
{
    VkPhysicalDeviceFeatures2 Features2;
    VkPhysicalDeviceVulkan11Features Vulkan11Features;
    VkPhysicalDeviceVulkan12Features Vulkan12Features;
    VkPhysicalDeviceVulkan13Features Vulkan13Features;
};

/// @brief The QueueFamilyInfo struct contains queue family indices for use by the device.
struct QueueFamilyInfo
{
    uint32_t GraphicsFamily;
    uint32_t ComputeFamily;
    uint32_t TransferFamily;
};

/// @brief Swapchain configuration as determined by physical device and surface.
struct SwapchainConfig
{
    uint32_t MinImageCount;
    VkExtent2D ImageExtent;
    VkSurfaceFormatKHR SurfaceFormat;
    VkPresentModeKHR PresentMode;
    VkSurfaceTransformFlagBitsKHR CurrentTransform;
};

/// @brief A CompiledShader contains shader SPIR-V bytecode and the associated shader stage.
struct CompiledShader
{
    ShaderStage Stage;
    IDxcBlob* Bytecode;
};

/// @brief Vulkan command list implementation.
struct BVulkanCommandListImpl
{
    BVulkanRenderBackendImpl const* RenderBackend; //> Needed for resource pool access.
    VkCommandPool CommandPool;
    VkCommandBuffer CommandBuffer;
};

/// @brief Vulkan render backend implementation.
struct BVulkanRenderBackendImpl
{
    /** Instance state */
    SDL_Window* Window; // Main window
    VkInstance Instance;
    VkDebugUtilsMessengerEXT DebugMessenger;
    VkSurfaceKHR Surface;
    
    /** Device state */
    VkPhysicalDevice PhysicalDevice;
    VkDevice Device;
    VmaAllocator Allocator;
    QueueFamilyInfo DeviceQueueFamilies;
    VkQueue GraphicsQueue;
    VkQueue ComputeQueue;
    VkQueue TransferQueue;

    /** Swap state */
    VkSwapchainKHR Swapchain;
    std::vector<VkImage> SwapchainImages;
    std::vector<VkImageView> SwapchainImageViews;

    /** Sync state */
    // TODO(nemjit001): Might need to store swap access semaphores
    // TODO(nemjit001): Size queue fences on frames in flight (provided by application?)
    std::vector<VkSemaphore> SwapImageReadySempahores; // Sized on swap image count
    VkFence GraphicsQueueFence;
    VkFence ComputeQueueFence;
    VkFence TransferQueueFence;

    /** Frame state */
    uint32_t SwapImageIdx;
    uint64_t FrameIndex;

    /** Resource pools */
    TRenderResourcePool<BGPUBuffer> BufferPool;
    TRenderResourcePool<BGPUTexture> TexturePool;
    TRenderResourcePool<BGPUPipelineState> PSOPool;
};

/// @brief Vulkan debug message callback.
/// @param Severity 
/// @param Type 
/// @param CallbackData 
/// @param UserData 
/// @return 
static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT,
    VkDebugUtilsMessageTypeFlagsEXT,
    VkDebugUtilsMessengerCallbackDataEXT const* CallbackData,
    void*
)
{
    BONSAI_LOG(LogVerbosity::Verbose, "[Vulkan] %s", CallbackData->pMessage);
    return VK_FALSE;
}

/// @brief Check if a list of instance layers is available for use.
/// @param LayerNames List of layers to check availability for.
/// @return 
static bool InstanceLayersAvailable(std::vector<char const*> const& LayerNames)
{
    uint32_t LayerCount = 0;
    vkEnumerateInstanceLayerProperties(&LayerCount, nullptr);
    std::vector<VkLayerProperties> AvailableLayers(LayerCount);
    vkEnumerateInstanceLayerProperties(&LayerCount, AvailableLayers.data());

    for (auto const& RequestedLayerName : LayerNames)
    {
        bool Available = false;
        for (auto const& AvailableLayer : AvailableLayers)
        {
            if (std::strcmp(AvailableLayer.layerName, RequestedLayerName) == 0)
            {
                Available = true;
                break;
            }
        }

        if (!Available) {
            return false;
        }
    }

    return true;
}

/// @brief Check if a list of instance extensions is available for use.
/// @param ExtensionNames List of extensions to check availability for.
/// @return 
static bool InstanceExtensionsAvailable(std::vector<char const*> const& ExtensionNames)
{
    uint32_t ExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, nullptr);
    std::vector<VkExtensionProperties> AvailableExtensions(ExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, AvailableExtensions.data());

    for (auto const& RequestedExtensionName : ExtensionNames)
    {
        bool Available = false;
        for (auto const& AvailableExtension : AvailableExtensions)
        {
            if (std::strcmp(AvailableExtension.extensionName, RequestedExtensionName) == 0)
            {
                Available = true;
                break;
            }
        }

        if (!Available) {
            return false;
        }
    }

    return true;
}

/// @brief Check if a list of device extensions is available for use.
/// @param PhysicalDevice Physical device to query extension availability for.
/// @param ExtensionNames List of extensions to check availability for.
/// @return 
static bool DeviceExtensionsAvailable(VkPhysicalDevice PhysicalDevice, std::vector<char const*> const& ExtensionNames)
{
    uint32_t ExtensionCount = 0;
    vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &ExtensionCount, nullptr);
    std::vector<VkExtensionProperties> AvailableExtensions(ExtensionCount);
    vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &ExtensionCount, AvailableExtensions.data());

    for (auto const& RequestedExtensionName : ExtensionNames)
    {
        bool Available = false;
        for (auto const& AvailableExtension : AvailableExtensions)
        {
            if (std::strcmp(AvailableExtension.extensionName, RequestedExtensionName) == 0)
            {
                Available = true;
                break;
            }
        }

        if (!Available) {
            return false;
        }
    }

    return true;
}

/// @brief Check if a physical device has support for presenting to a surface on any queue family.
/// @param PhysicalDevice 
/// @param DefaultSurface 
/// @return 
static bool PhysicalDeviceHasSurfaceSupport(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR DefaultSurface)
{
    uint32_t QueueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueCount, nullptr);
    std::vector<VkQueueFamilyProperties> QueueFamilies(QueueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueCount, QueueFamilies.data());

    bool HasSurfaceSupport = false;
    for (uint32_t i = 0; i < QueueCount; i++)
    {
        VkBool32 SurfaceSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, i, DefaultSurface, &SurfaceSupport);
        HasSurfaceSupport |= SurfaceSupport;
    }

    return HasSurfaceSupport;
}

/// @brief Find a single queue family using a set of required and ignored flags.
/// @param QueueFamilies 
/// @param PhysicalDevice 
/// @param DefaultSurface Optional surface to query for support on queue.
/// @param RequiredFlags Required queue flags.
/// @param FilterFlags Filter flags, excludes queues with given flag set.
/// @return 
static uint32_t FindQueueFamily(
    std::vector<VkQueueFamilyProperties> const& QueueFamilies,
    VkPhysicalDevice PhysicalDevice,
    VkSurfaceKHR DefaultSurface,
    VkQueueFlags RequiredFlags,
    VkQueueFlags FilterFlags
)
{
    uint32_t const QueueCount = static_cast<uint32_t>(QueueFamilies.size());
    for (uint32_t i = 0; i < QueueCount; i++)
    {
        VkBool32 SurfaceSupport = VK_TRUE; // If no surface was passed support is implicit.
        if (DefaultSurface != VK_NULL_HANDLE) {
            vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, i, DefaultSurface, &SurfaceSupport);
        }

        if (SurfaceSupport != VK_FALSE
            && (QueueFamilies[i].queueFlags & RequiredFlags) == RequiredFlags
            && (QueueFamilies[i].queueFlags & FilterFlags) == 0
        ) {
            return i;
        }
    }

    return VK_QUEUE_FAMILY_IGNORED;
}

/// @brief Get device queue families for a physical device.
/// @param PhysicalDevice 
/// @param DefaultSurface 
/// @param OutQueueFamilies 
/// @return A boolean indicating that all device queues were found successfully.
static bool GetDeviceQueueFamilies(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR DefaultSurface, QueueFamilyInfo& OutQueueFamilies)
{
    uint32_t QueueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueCount, nullptr);
    std::vector<VkQueueFamilyProperties> QueueFamilies(QueueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueCount, QueueFamilies.data());

    // FIXME(nemjit001): This assumes that async queues are available, add fallback selection on single queue too
    OutQueueFamilies.GraphicsFamily = FindQueueFamily(QueueFamilies, PhysicalDevice, DefaultSurface, VK_QUEUE_GRAPHICS_BIT, 0);
    OutQueueFamilies.ComputeFamily = FindQueueFamily(QueueFamilies, PhysicalDevice, VK_NULL_HANDLE, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_GRAPHICS_BIT);
    OutQueueFamilies.TransferFamily = FindQueueFamily(QueueFamilies, PhysicalDevice, VK_NULL_HANDLE, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT);

    return OutQueueFamilies.GraphicsFamily != VK_QUEUE_FAMILY_IGNORED
        && OutQueueFamilies.ComputeFamily != VK_QUEUE_FAMILY_IGNORED
        && OutQueueFamilies.TransferFamily != VK_QUEUE_FAMILY_IGNORED;
}

/// @brief Get unique queue family indices for a QueueFamilyInfo structure.
/// @param QueueFamilies 
/// @return 
static std::vector<uint32_t> GetUniqueQueueFamilies(QueueFamilyInfo const& QueueFamilies)
{
    std::vector<uint32_t> QueueIndices = {
        QueueFamilies.GraphicsFamily,
        QueueFamilies.ComputeFamily,
        QueueFamilies.TransferFamily,
    };

    // Sorts values, then erases consecutive duplicate indices. Results in a list of deduplicated queue families.
    std::sort(std::begin(QueueIndices), std::end(QueueIndices));
    QueueIndices.erase(
        std::unique(std::begin(QueueIndices), std::end(QueueIndices)),
        std::end(QueueIndices)
    );

    return QueueIndices;
}

/// @brief Select a physical device. Feature requirements are resolved in this function.
/// @param Instance 
/// @param DefaultSurface 
/// @param RequiredExtensions 
/// @param OutEnabledFeatures 
/// @return 
static VkPhysicalDevice SelectPhysicalDevice(
    VkInstance Instance,
    VkSurfaceKHR DefaultSurface,
    std::vector<char const*> const& RequiredExtensions,
    DeviceFeatures& OutEnabledFeatures
)
{
    uint32_t DeviceCount = 0;
    vkEnumeratePhysicalDevices(Instance, &DeviceCount, nullptr);
    std::vector<VkPhysicalDevice> PhysicalDevices(DeviceCount);
    vkEnumeratePhysicalDevices(Instance, &DeviceCount, PhysicalDevices.data());

    OutEnabledFeatures = DeviceFeatures{};
    OutEnabledFeatures.Features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    OutEnabledFeatures.Vulkan11Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
    OutEnabledFeatures.Vulkan12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    OutEnabledFeatures.Vulkan13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    OutEnabledFeatures.Features2.pNext = &OutEnabledFeatures.Vulkan11Features;
    OutEnabledFeatures.Vulkan11Features.pNext = &OutEnabledFeatures.Vulkan12Features;
    OutEnabledFeatures.Vulkan12Features.pNext = &OutEnabledFeatures.Vulkan13Features;
    OutEnabledFeatures.Vulkan13Features.pNext = nullptr;

    for (auto const& PhysicalDevice : PhysicalDevices)
    {
        VkPhysicalDeviceProperties2 DeviceProperties{};
        DeviceProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

        vkGetPhysicalDeviceProperties2(PhysicalDevice, &DeviceProperties);
        if (DeviceProperties.properties.apiVersion < BONSAI_VULKAN_API_VERSION) {
            continue;
        }

        vkGetPhysicalDeviceFeatures2(PhysicalDevice, &OutEnabledFeatures.Features2);
        if (
            /** Vulkan 1.0 features */
            OutEnabledFeatures.Features2.features.samplerAnisotropy == VK_FALSE
            || OutEnabledFeatures.Features2.features.multiDrawIndirect == VK_FALSE
            /** Vulkan 1.2 features */
            || OutEnabledFeatures.Vulkan12Features.bufferDeviceAddress == VK_FALSE
            || OutEnabledFeatures.Vulkan12Features.descriptorBindingPartiallyBound == VK_FALSE
            || OutEnabledFeatures.Vulkan12Features.descriptorIndexing == VK_FALSE
            || OutEnabledFeatures.Vulkan12Features.drawIndirectCount == VK_FALSE
            || OutEnabledFeatures.Vulkan12Features.scalarBlockLayout == VK_FALSE
            /** Vulkan 1.3 features */
            || OutEnabledFeatures.Vulkan13Features.dynamicRendering == VK_FALSE
            || OutEnabledFeatures.Vulkan13Features.synchronization2 == VK_FALSE
        ) {
            continue;
        }

        if (!DeviceExtensionsAvailable(PhysicalDevice, RequiredExtensions)) {
            continue;
        }

        // Query surface and queue setup support
        QueueFamilyInfo RequestedQueueFamilies{};
        if (!PhysicalDeviceHasSurfaceSupport(PhysicalDevice, DefaultSurface)
            || !GetDeviceQueueFamilies(PhysicalDevice, DefaultSurface, RequestedQueueFamilies)) {
            continue;
        }

        return PhysicalDevice;
    }

    return VK_NULL_HANDLE;
}

/// @brief Get a swapchain configuration from a physical device, surface, and window.
/// @param PhysicalDevice 
/// @param Surface 
/// @param Window 
/// @return 
static SwapchainConfig GetSwapchainConfig(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface, SDL_Window* Window)
{
    VkSurfaceCapabilitiesKHR SurfaceCapabilities{};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, Surface, &SurfaceCapabilities);

    uint32_t SurfaceFormatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &SurfaceFormatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> SurfaceFormats(SurfaceFormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &SurfaceFormatCount, SurfaceFormats.data());

    uint32_t PresentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &PresentModeCount, nullptr);
    std::vector<VkPresentModeKHR> PresentModes(PresentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &PresentModeCount, PresentModes.data());

    int WindowWidth = 0, WindowHeight = 0;
    SDL_GetWindowSizeInPixels(Window, &WindowWidth, &WindowHeight);

    SwapchainConfig SwapConfig{};
    SwapConfig.MinImageCount = SurfaceCapabilities.maxImageCount != 0 ? std::min(SurfaceCapabilities.minImageCount + 1, SurfaceCapabilities.maxImageCount) : SurfaceCapabilities.minImageCount + 1;
    SwapConfig.ImageExtent = VkExtent2D{ static_cast<uint32_t>(WindowWidth), static_cast<uint32_t>(WindowHeight) };
    SwapConfig.SurfaceFormat = SurfaceFormats[0]; // Pick first format for now, should function fine
    SwapConfig.PresentMode = VK_PRESENT_MODE_FIFO_KHR;
    SwapConfig.CurrentTransform = SurfaceCapabilities.currentTransform;

    return SwapConfig;
}

/// @brief Create a swap chain using a swapchain configuration.
/// @param Device 
/// @param Surface 
/// @param OldSwapchain Optional old swap chain, destroyed automatically if not VK_NULL_HANDLE.
/// @param SwapConfig Swapchain configuration to use for swapchain creation.
/// @param OutSwapchain 
/// @return A boolean indicating successful swapchain creation.
static bool CreateSwapchain(VkDevice Device, VkSurfaceKHR Surface, VkSwapchainKHR OldSwapchain, SwapchainConfig const& SwapConfig, VkSwapchainKHR& OutSwapchain)
{
    VkSwapchainCreateInfoKHR SwapchainCreateInfo{};
    SwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    SwapchainCreateInfo.pNext = nullptr;
    SwapchainCreateInfo.flags = 0;
    SwapchainCreateInfo.surface = Surface;
    SwapchainCreateInfo.minImageCount = SwapConfig.MinImageCount;
    SwapchainCreateInfo.imageFormat = SwapConfig.SurfaceFormat.format;
    SwapchainCreateInfo.imageColorSpace = SwapConfig.SurfaceFormat.colorSpace;
    SwapchainCreateInfo.imageExtent = SwapConfig.ImageExtent;
    SwapchainCreateInfo.imageArrayLayers = 1;
    SwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
        | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    SwapchainCreateInfo.queueFamilyIndexCount = 0;
    SwapchainCreateInfo.pQueueFamilyIndices = nullptr;
    SwapchainCreateInfo.preTransform = SwapConfig.CurrentTransform;
    SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // Assume we won't need any alpha compositing for the window.
    SwapchainCreateInfo.presentMode = SwapConfig.PresentMode;
    SwapchainCreateInfo.clipped = VK_FALSE;
    SwapchainCreateInfo.oldSwapchain = OldSwapchain;

    VkSwapchainKHR Swapchain = VK_NULL_HANDLE;
    if (vkCreateSwapchainKHR(Device, &SwapchainCreateInfo, nullptr, &Swapchain) != VK_SUCCESS) {
        return false;
    }

    vkDestroySwapchainKHR(Device, OldSwapchain, nullptr);
    OutSwapchain = Swapchain;
    return true;
}

/// @brief Create swapchain image views for swapchain images.
/// @param Device 
/// @param SwapConfig Swapchain configuration used to create the swap chain that owns the swap images.
/// @param SwapchainImages 
/// @param OutImageViews 
/// @return 
static bool CreateSwapchainImageViews(VkDevice Device, SwapchainConfig const& SwapConfig, std::vector<VkImage> const& SwapchainImages, std::vector<VkImageView>& OutImageViews)
{
    uint32_t const SwapImageCount = static_cast<uint32_t>(SwapchainImages.size());
    OutImageViews.clear();
    OutImageViews.reserve(SwapImageCount);
    for (auto const& Image : SwapchainImages)
    {
        VkImageViewCreateInfo ViewCreateInfo{};
        ViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ViewCreateInfo.pNext = nullptr;
        ViewCreateInfo.flags = 0;
        ViewCreateInfo.image = Image;
        ViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        ViewCreateInfo.format = SwapConfig.SurfaceFormat.format;
        ViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        ViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        ViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        ViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        ViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ViewCreateInfo.subresourceRange.baseMipLevel = 0;
        ViewCreateInfo.subresourceRange.levelCount = 1;
        ViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        ViewCreateInfo.subresourceRange.layerCount = 1;

        VkImageView SwapImageView = VK_NULL_HANDLE;
        if (vkCreateImageView(Device, &ViewCreateInfo, nullptr, &SwapImageView) != VK_SUCCESS) {
            return false;
        }

        OutImageViews.push_back(SwapImageView);
    }

    return true;
}

/// @brief Create a Vulkan shader module from SPIRV bytecode.
/// @param Device 
/// @param ByteCode 
/// @param CodeSize 
/// @return 
static VkShaderModule CreateShaderModule(VkDevice Device, uint32_t const* ByteCode, size_t CodeSize)
{
    VkShaderModuleCreateInfo ShaderModuleCreateInfo{};
    ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ShaderModuleCreateInfo.pNext = nullptr;
    ShaderModuleCreateInfo.flags = 0;
    ShaderModuleCreateInfo.pCode = ByteCode;
    ShaderModuleCreateInfo.codeSize = CodeSize;

    VkShaderModule ShaderModule = VK_NULL_HANDLE;
    if (vkCreateShaderModule(Device, &ShaderModuleCreateInfo, nullptr, &ShaderModule) != VK_SUCCESS) {
        return VK_NULL_HANDLE;
    }

    return ShaderModule;
}

/// @brief Compile a shader into SPIR-V bytecode.
/// @param ShaderSource 
/// @param OutShaderBytecode 
/// @return 
static bool CompileShader(BShaderSource const& ShaderSource, IDxcBlob** OutShaderBytecode)
{
    // TODO(nemjit001): Push additional stage-specific compilation args.
    char const* TargetProfile = nullptr;
    std::vector<LPCWSTR> CompilationArgs(std::begin(DefaultShaderCompilationArgs), std::end(DefaultShaderCompilationArgs));
    switch(ShaderSource.Stage)
    {
    case ShaderStage_Vertex:
        TargetProfile = BONSAI_VK_VERTEX_SHADER_TARGET_PROFILE;
        break;
    case ShaderStage_Hull:
        TargetProfile = BONSAI_VK_HULL_SHADER_TARGET_PROFILE;
        break;
    case ShaderStage_Domain:
        TargetProfile = BONSAI_VK_DOMAIN_SHADER_TARGET_PROFILE;
        break;
    case ShaderStage_Geometry:
        TargetProfile = BONSAI_VK_GEOMETRY_SHADER_TARGET_PROFILE;
        break;
    case ShaderStage_Pixel:
        TargetProfile = BONSAI_VK_PIXEL_SHADER_TARGET_PROFILE;
        break;
    case ShaderStage_Compute:
        TargetProfile = BONSAI_VK_COMPUTE_SHADER_TARGET_PROFILE;
        break;
    default:
        return false;
    }

    // TODO(nemjit001): Push additional shader defines and default include dirs
    BShaderCompilerArgs CompilerArgs{};
    CompilerArgs.Arguments = CompilationArgs.data();
    CompilerArgs.ArgumentCount = CompilationArgs.size();

    return SHADER_COMPILER_RESULT_OK == BShaderCompiler::CompileFile(ShaderSource.ShaderPath, ShaderSource.Entrypoint, TargetProfile, CompilerArgs, OutShaderBytecode);
}

/// @brief Reflect pipeline layout from a list of compiled shaders.
/// @param Device
/// @param Shaders 
/// @param OutPushConstantRanges 
/// @param OutDescriptorSetLayouts 
/// @return 
static bool ReflectPipelineLayout(
    VkDevice Device,
    std::vector<CompiledShader> const& Shaders,
    std::vector<BGPUPipelineLocalSize>& OutLocalSizes,
    std::vector<VkPushConstantRange>& OutPushConstantRanges,
    std::vector<VkDescriptorSetLayout>& OutDescriptorSetLayouts
)
{
    // Create reflect modules
    std::vector<SpvReflectShaderModule> ReflectModules;
    ReflectModules.reserve(Shaders.size());
    for (auto const& Shader : Shaders)
    {
        SpvReflectShaderModule Module{};
        spvReflectCreateShaderModule2(SPV_REFLECT_MODULE_FLAG_NONE, Shader.Bytecode->GetBufferSize(), Shader.Bytecode->GetBufferPointer(), &Module);    
        ReflectModules.push_back(Module);
    }

    // Parse dispatch sizes, descriptor set layouts, and push constant ranges
    OutLocalSizes.clear();
    OutPushConstantRanges.clear();
    std::vector<std::vector<VkDescriptorSetLayoutBinding>> DescriptorSetBindings;
    uint32_t MaxDescriptorSetIdx = 0;
    for (auto const& Module : ReflectModules)
    {
        // Store 1st entrypoint dispatch size for shader
        BGPUPipelineLocalSize LocalSize{};
        LocalSize.SizeX = Module.entry_points[Module.entry_point_id].local_size.x;
        LocalSize.SizeY = Module.entry_points[Module.entry_point_id].local_size.y;
        LocalSize.SizeZ = Module.entry_points[Module.entry_point_id].local_size.z;

        OutLocalSizes.push_back(LocalSize);

        // Parse per-entrypoint descriptor set layouts & push constants
        for (uint32_t EntrypointIdx = 0; EntrypointIdx < Module.entry_point_count; EntrypointIdx++)
        {
            // Get entrypoint in reflect module
            SpvReflectEntryPoint const& Entrypoint = Module.entry_points[EntrypointIdx];

            // Parse push constant ranges in entrypoint. Each shader stage entrypoint may only have 1 associated push constant range.
            for (uint32_t PushConstantIdx = 0; PushConstantIdx < Entrypoint.used_push_constant_count; PushConstantIdx++)
            {
                SpvReflectBlockVariable const& PushConstantBlock = Module.push_constant_blocks[Entrypoint.used_push_constants[PushConstantIdx]];
                auto const Iter = std::find_if(std::begin(OutPushConstantRanges), std::end(OutPushConstantRanges), [&](VkPushConstantRange const& PushConstant) {
                    return PushConstant.offset == PushConstantBlock.absolute_offset && PushConstant.size == PushConstantBlock.padded_size;
                });
                
                if (Iter != std::end(OutPushConstantRanges))
                {
                    VkPushConstantRange& StoredPushConstant = *Iter;
                    StoredPushConstant.stageFlags |= Entrypoint.shader_stage; // Only need to update stage flags here
                }
                else
                {
                    VkPushConstantRange NewPushConstantRange{};
                    NewPushConstantRange.offset = PushConstantBlock.absolute_offset;
                    NewPushConstantRange.size = PushConstantBlock.padded_size;
                    NewPushConstantRange.stageFlags = Entrypoint.shader_stage;

                    OutPushConstantRanges.push_back(NewPushConstantRange);
                }
            }

            // Parse descriptor sets in entrypoint
            for (uint32_t DescriptorSetIdx = 0; DescriptorSetIdx < Entrypoint.descriptor_set_count; DescriptorSetIdx++)
            {
                // Resize descriptor sets array
                SpvReflectDescriptorSet const& DescriptorSet = Entrypoint.descriptor_sets[DescriptorSetIdx];
                MaxDescriptorSetIdx = std::max(MaxDescriptorSetIdx, DescriptorSet.set);
                DescriptorSetBindings.resize(MaxDescriptorSetIdx);

                // Parse descriptor bindings in entrypoint
                std::vector<VkDescriptorSetLayoutBinding>& CurrentSetBindings = DescriptorSetBindings[DescriptorSet.set];
                for (uint32_t DescriptorBindingIdx = 0; DescriptorBindingIdx < DescriptorSet.binding_count; DescriptorBindingIdx++)
                {
                    SpvReflectDescriptorBinding const* DescriptorBinding = DescriptorSet.bindings[DescriptorBindingIdx];
                    auto const Iter = std::find_if(std::begin(CurrentSetBindings), std::end(CurrentSetBindings), [&](VkDescriptorSetLayoutBinding const& Binding) {
                        return Binding.binding == DescriptorBinding->binding;
                    });

                    if (Iter != std::end(CurrentSetBindings))
                    {
                        VkDescriptorSetLayoutBinding& StoredDescriptorBinding = *Iter;
                        StoredDescriptorBinding.stageFlags |= Entrypoint.shader_stage; // Only need to update used stage flags here
                    }
                    else
                    {
                        VkDescriptorSetLayoutBinding NewDescriptorBinding{};
                        NewDescriptorBinding.binding = DescriptorBinding->binding;
                        NewDescriptorBinding.descriptorType = static_cast<VkDescriptorType>(DescriptorBinding->descriptor_type);
                        NewDescriptorBinding.descriptorCount = DescriptorBinding->count;
                        NewDescriptorBinding.stageFlags = Entrypoint.shader_stage;
                        NewDescriptorBinding.pImmutableSamplers = nullptr;
                        
                        CurrentSetBindings.push_back(NewDescriptorBinding);
                    }
                }
            }
        }
    }

    // Destroy reflect modules :)
    for (auto& Module : ReflectModules) {
        spvReflectDestroyShaderModule(&Module);
    }

    // Create descriptor set layouts
    OutDescriptorSetLayouts.clear();
    OutDescriptorSetLayouts.reserve(DescriptorSetBindings.size());
    for (auto const& Bindings : DescriptorSetBindings)
    {
        VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo{};
        DescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        DescriptorSetLayoutCreateInfo.pNext = nullptr;
        DescriptorSetLayoutCreateInfo.flags = 0; // TODO(nemjit001): Check if update after bind is needed (bindless rendering etc.)
        DescriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(Bindings.size());
        DescriptorSetLayoutCreateInfo.pBindings = Bindings.data();

        VkDescriptorSetLayout DescriptorSetLayout = VK_NULL_HANDLE;
        if (vkCreateDescriptorSetLayout(Device, &DescriptorSetLayoutCreateInfo, nullptr, &DescriptorSetLayout)) {
            return false;
        }

        OutDescriptorSetLayouts.push_back(DescriptorSetLayout);
    }

    return true;
}

/*
 * VulkanCommandList Implementation
 */

BVulkanCommandList::BVulkanCommandList(BVulkanRenderBackendImpl const* RenderBackendImpl)
    :
    pImpl(std::make_unique<BVulkanCommandListImpl>())
{
    pImpl->RenderBackend = RenderBackendImpl;
    pImpl->CommandPool = VK_NULL_HANDLE;
    pImpl->CommandBuffer = VK_NULL_HANDLE;
}

BVulkanCommandList::~BVulkanCommandList()
{
    vkFreeCommandBuffers(pImpl->RenderBackend->Device, pImpl->CommandPool, 1, &pImpl->CommandBuffer);
}

bool BVulkanCommandList::Reset()
{
    if (vkResetCommandBuffer(pImpl->CommandBuffer, 0) != VK_SUCCESS) {
        return false;
    }

    VkCommandBufferBeginInfo BeginInfo{};
    BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    BeginInfo.pNext = nullptr;
    BeginInfo.flags = 0;
    BeginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(pImpl->CommandBuffer, &BeginInfo) != VK_SUCCESS) {
        return false;
    }

    return true;
}

bool BVulkanCommandList::Close()
{
    return VK_SUCCESS == vkEndCommandBuffer(pImpl->CommandBuffer);
}

void BVulkanCommandList::SetPipelineState(BGPUPipelineStateHandle PipelineState)
{
    BGPUPipelineState PSOResource{};
    if (!pImpl->RenderBackend->PSOPool.GetResource(PipelineState, PSOResource))
    {
        BONSAI_LOG(LogVerbosity::Warning, "Skipping command: attempted to set unknown PSO (PSO handle %d:%d)", PipelineState.ResourceId, PipelineState.ResourceVersion);
        return;
    }

    vkCmdBindPipeline(pImpl->CommandBuffer, PSOResource.BindPoint, PSOResource.Pipeline);
}

void BVulkanCommandList::Dispatch(uint32_t X, uint32_t Y, uint32_t Z)
{
    vkCmdDispatch(pImpl->CommandBuffer, X, Y, Z);
}

/*
 * VulkanRenderBackend Implementation
 */

BVulkanRenderBackend::BVulkanRenderBackend()
    :
    pImpl(std::make_unique<BVulkanRenderBackendImpl>())
{
    //
}

BVulkanRenderBackend::~BVulkanRenderBackend() = default;

RendererResult BVulkanRenderBackend::Init()
{
    if (volkInitialize() != VK_SUCCESS) {
        return RENDERER_RESULT_SYMBOL_LOADING_BAD_INIT;
    }

    SDL_Window* Window = SDL_CreateWindow("Vulkan Window", 1280, 720, SDL_WINDOW_VULKAN);
    if (!Window) {
        return RENDERER_RESULT_WINDOW_BAD_INIT;
    }

    // Initialize Vulkan instance with required extensions
    VkApplicationInfo AppInfo{};
    AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    AppInfo.pNext = nullptr;
    AppInfo.pApplicationName = "Bonsai Viewer";
    AppInfo.applicationVersion = 0;
    AppInfo.pEngineName = "Bonsai Viewer";
    AppInfo.engineVersion = 0;
    AppInfo.apiVersion = BONSAI_VULKAN_API_VERSION;

    uint32_t WindowExtensionCount = 0;
    char const** WindowExtensionNames = const_cast<char const**>(SDL_Vulkan_GetInstanceExtensions(&WindowExtensionCount));
    std::vector<char const*> InstanceLayerNames;
    std::vector<char const*> InstanceExtensionNames(WindowExtensionNames, WindowExtensionNames + WindowExtensionCount);
    if constexpr (RendererDebugEnabled)
    {   // Enable debug extension and layer names when debug is enabled
        InstanceLayerNames.insert(
            std::end(InstanceLayerNames),
            std::begin(DefaultInstanceDebugLayerNames),
            std::end(DefaultInstanceDebugLayerNames)
        );

        InstanceExtensionNames.insert(
            std::end(InstanceExtensionNames),
            std::begin(DefaultInstanceDebugExtensionNames),
            std::end(DefaultInstanceDebugExtensionNames)
        );
    }

    if (!InstanceLayersAvailable(InstanceLayerNames) || !InstanceExtensionsAvailable(InstanceExtensionNames)) {
        return RENDERER_RESULT_GFX_API_BAD_INIT;
    }

    VkDebugUtilsMessengerCreateInfoEXT DebugMessengerCreateInfo{};
    DebugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    DebugMessengerCreateInfo.pNext = nullptr;
    DebugMessengerCreateInfo.flags = 0;
    DebugMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    DebugMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;
    DebugMessengerCreateInfo.pfnUserCallback = VulkanDebugCallback;
    DebugMessengerCreateInfo.pUserData = nullptr;

    VkInstanceCreateInfo InstanceCreateInfo{};
    InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    InstanceCreateInfo.pNext = nullptr;
    InstanceCreateInfo.flags = 0;
    InstanceCreateInfo.pApplicationInfo = &AppInfo;
    InstanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(InstanceLayerNames.size());
    InstanceCreateInfo.ppEnabledLayerNames = InstanceLayerNames.data();
    InstanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(InstanceExtensionNames.size());
    InstanceCreateInfo.ppEnabledExtensionNames = InstanceExtensionNames.data();
    if constexpr (RendererDebugEnabled) { // Enable debug messages for startup
        InstanceCreateInfo.pNext = &DebugMessengerCreateInfo;
    }

    VkInstance Instance = VK_NULL_HANDLE;
    if (vkCreateInstance(&InstanceCreateInfo, nullptr, &Instance) != VK_SUCCESS) {
        return RENDERER_RESULT_GFX_API_BAD_INIT;
    }
    volkLoadInstance(Instance);

    VkDebugUtilsMessengerEXT DebugMessenger = VK_NULL_HANDLE;
    if constexpr (RendererDebugEnabled)
    {
        if (vkCreateDebugUtilsMessengerEXT(Instance, &DebugMessengerCreateInfo, nullptr, &DebugMessenger) != VK_SUCCESS) {
            return RENDERER_RESULT_GFX_API_BAD_INIT;
        }
    }

    VkSurfaceKHR Surface = VK_NULL_HANDLE;
    if (!SDL_Vulkan_CreateSurface(Window, Instance, nullptr, &Surface)) {
        return RENDERER_RESULT_GFX_API_BAD_INIT;
    }

    // Select physical device & create logical device
    std::vector<char const*> DeviceExtensionNames(std::begin(DefaultDeviceExtensionNames), std::end(DefaultDeviceExtensionNames));
    DeviceFeatures EnabledDeviceFeatures{};
    VkPhysicalDevice PhysicalDevice = SelectPhysicalDevice(Instance, Surface, DeviceExtensionNames, EnabledDeviceFeatures);
    if (!PhysicalDevice) {
        return RENDERER_RESULT_UNSUPPORTED_GFX_DEVICE;
    }

    QueueFamilyInfo DeviceQueueFamilies{};
    if (!GetDeviceQueueFamilies(PhysicalDevice, Surface, DeviceQueueFamilies)) {
        return RENDERER_RESULT_UNSUPPORTED_GFX_DEVICE;
    }

    std::vector<uint32_t> const QueueFamilyIndices = GetUniqueQueueFamilies(DeviceQueueFamilies);
    std::vector<VkDeviceQueueCreateInfo> QueueCreateInfos{};
    QueueCreateInfos.reserve(QueueFamilyIndices.size());
    float QueuePriorities[] = { 1.0F };
    for (auto const& QueueIndex : QueueFamilyIndices)
    {
        VkDeviceQueueCreateInfo QueueCreateInfo{};
        QueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        QueueCreateInfo.pNext = nullptr;
        QueueCreateInfo.flags = 0;
        QueueCreateInfo.queueFamilyIndex = QueueIndex;
        QueueCreateInfo.queueCount = 1; // Assume just 1 queue per family for now...
        QueueCreateInfo.pQueuePriorities = QueuePriorities;

        QueueCreateInfos.push_back(QueueCreateInfo);
    }

    VkDeviceCreateInfo DeviceCreateInfo{};
    DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    DeviceCreateInfo.pNext = &EnabledDeviceFeatures.Features2;
    DeviceCreateInfo.flags = 0;
    DeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(QueueCreateInfos.size());
    DeviceCreateInfo.pQueueCreateInfos = QueueCreateInfos.data();
    DeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(DeviceExtensionNames.size());
    DeviceCreateInfo.ppEnabledExtensionNames = DeviceExtensionNames.data();
    DeviceCreateInfo.pEnabledFeatures = nullptr;

    VkDevice Device = VK_NULL_HANDLE;
    if (vkCreateDevice(PhysicalDevice, &DeviceCreateInfo, nullptr, &Device) != VK_SUCCESS) {
        return RENDERER_RESULT_GFX_API_BAD_INIT;
    }
    volkLoadDevice(Device);

    // Create VMA allocator
    VmaAllocatorCreateInfo AllocatorCreateInfo{};
    AllocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT
        | VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    AllocatorCreateInfo.instance = Instance;
    AllocatorCreateInfo.physicalDevice = PhysicalDevice;
    AllocatorCreateInfo.device = Device;
    AllocatorCreateInfo.vulkanApiVersion = BONSAI_VULKAN_API_VERSION;

    VmaVulkanFunctions VmaFunctionPointers{};
    if (vmaImportVulkanFunctionsFromVolk(&AllocatorCreateInfo, &VmaFunctionPointers) != VK_SUCCESS) {
        return RENDERER_RESULT_GFX_API_BAD_INIT;
    }
    AllocatorCreateInfo.pVulkanFunctions = &VmaFunctionPointers;

    VmaAllocator Allocator = nullptr;
    if (vmaCreateAllocator(&AllocatorCreateInfo, &Allocator) != VK_SUCCESS) {
        return RENDERER_RESULT_GFX_API_BAD_INIT;
    }

    // Get queue families from device
    VkQueue GraphicsQueue = VK_NULL_HANDLE;
    VkQueue ComputeQueue = VK_NULL_HANDLE;
    VkQueue TransferQueue = VK_NULL_HANDLE;
    vkGetDeviceQueue(Device, DeviceQueueFamilies.GraphicsFamily, 0, &GraphicsQueue);
    vkGetDeviceQueue(Device, DeviceQueueFamilies.ComputeFamily, 0, &ComputeQueue);
    vkGetDeviceQueue(Device, DeviceQueueFamilies.TransferFamily, 0, &TransferQueue);

    // Create swap state
    SwapchainConfig const SwapConfig = GetSwapchainConfig(PhysicalDevice, Surface, Window);
    VkSwapchainKHR Swapchain = VK_NULL_HANDLE;
    if (!CreateSwapchain(Device, Surface, VK_NULL_HANDLE, SwapConfig, Swapchain)) {
        return RENDERER_RESULT_SWAPCHAIN_BAD_INIT;
    }

    uint32_t SwapImageCount = 0;
    vkGetSwapchainImagesKHR(Device, Swapchain, &SwapImageCount, nullptr);
    std::vector<VkImage> SwapchainImages(SwapImageCount);
    vkGetSwapchainImagesKHR(Device, Swapchain, &SwapImageCount, SwapchainImages.data());

    std::vector<VkImageView> SwapchainImageViews{};
    if (!CreateSwapchainImageViews(Device, SwapConfig, SwapchainImages, SwapchainImageViews)) {
        return RENDERER_RESULT_SWAPCHAIN_BAD_INIT;
    }

    // Create sync state
    std::vector<VkSemaphore> SwapImageReadySemaphores(SwapImageCount, VK_NULL_HANDLE);
    for (uint32_t i = 0; i < SwapImageCount; i++)
    {
        VkSemaphoreCreateInfo SemaphoreCreateInfo{};
        SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        SemaphoreCreateInfo.pNext = nullptr;
        SemaphoreCreateInfo.flags = 0;

        if (vkCreateSemaphore(Device, &SemaphoreCreateInfo, nullptr, &SwapImageReadySemaphores[i]) != VK_SUCCESS) {
            return RENDERER_RESULT_SYNC_STATE_BAD_INIT;
        }
    }

    VkFenceCreateInfo QueueFenceCreateInfo{};
    QueueFenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    QueueFenceCreateInfo.pNext = nullptr;
    QueueFenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    
    VkFence GraphicsQueueFence = VK_NULL_HANDLE;
    VkFence ComputeQueueFence = VK_NULL_HANDLE;
    VkFence TransferQueueFence = VK_NULL_HANDLE;
    if (vkCreateFence(Device, &QueueFenceCreateInfo, nullptr, &GraphicsQueueFence) != VK_SUCCESS
        || vkCreateFence(Device, &QueueFenceCreateInfo, nullptr, &ComputeQueueFence) != VK_SUCCESS
        || vkCreateFence(Device, &QueueFenceCreateInfo, nullptr, &TransferQueueFence) != VK_SUCCESS
    ) {
        return RENDERER_RESULT_SYNC_STATE_BAD_INIT;
    }

    pImpl->Window = Window;
    pImpl->Instance = Instance;
    pImpl->DebugMessenger = DebugMessenger;
    pImpl->Surface = Surface;
    pImpl->PhysicalDevice = PhysicalDevice;
    pImpl->Device = Device;
    pImpl->Allocator = Allocator;
    pImpl->DeviceQueueFamilies = DeviceQueueFamilies;
    pImpl->GraphicsQueue = GraphicsQueue;
    pImpl->ComputeQueue = ComputeQueue;
    pImpl->TransferQueue = TransferQueue;
    pImpl->Swapchain = Swapchain;
    pImpl->SwapchainImages = SwapchainImages;
    pImpl->SwapchainImageViews = SwapchainImageViews;
    pImpl->SwapImageReadySempahores = SwapImageReadySemaphores;
    pImpl->GraphicsQueueFence = GraphicsQueueFence;
    pImpl->ComputeQueueFence = ComputeQueueFence;
    pImpl->TransferQueueFence = TransferQueueFence;
    pImpl->SwapImageIdx = 0;
    pImpl->FrameIndex = 0;
    return RENDERER_RESULT_OK;
}

void BVulkanRenderBackend::Shutdown()
{
    // Ensure any work on the GPU finishes before cleanup starts
    vkDeviceWaitIdle(pImpl->Device);

    // Clean up sync state
    vkDestroyFence(pImpl->Device, pImpl->TransferQueueFence, nullptr);
    vkDestroyFence(pImpl->Device, pImpl->ComputeQueueFence, nullptr);
    vkDestroyFence(pImpl->Device, pImpl->GraphicsQueueFence, nullptr);
    for (auto const& Semaphore : pImpl->SwapImageReadySempahores) {
        vkDestroySemaphore(pImpl->Device, Semaphore, nullptr);
    }

    // Clean up swap state
    for (auto const& View : pImpl->SwapchainImageViews) {
        vkDestroyImageView(pImpl->Device, View, nullptr);
    }
    vkDestroySwapchainKHR(pImpl->Device, pImpl->Swapchain, nullptr);

    // Clean up device state
    vmaDestroyAllocator(pImpl->Allocator);
    vkDestroyDevice(pImpl->Device, nullptr);

    // Clean up instance state
    vkDestroySurfaceKHR(pImpl->Instance, pImpl->Surface, nullptr);
    if constexpr (RendererDebugEnabled) {
        vkDestroyDebugUtilsMessengerEXT(pImpl->Instance, pImpl->DebugMessenger, nullptr);
    }
    vkDestroyInstance(pImpl->Instance, nullptr);
    SDL_DestroyWindow(pImpl->Window);
    volkFinalize();
}

void BVulkanRenderBackend::NewFrame()
{
    // Wait for graphics queue to be idle
    vkWaitForFences(pImpl->Device, 1, &pImpl->GraphicsQueueFence, VK_TRUE, UINT64_MAX);

    // Acquire next swap image
    // TODO(nemjit001): Sync w/ graphics queue using semaphores (e.g. ping/pong semaphores between submissions?)
    vkAcquireNextImageKHR(pImpl->Device, pImpl->Swapchain, UINT64_MAX, VK_NULL_HANDLE, VK_NULL_HANDLE, &pImpl->SwapImageIdx);
}

void BVulkanRenderBackend::Present()
{
    VkPresentInfoKHR PresentInfo{};
    PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    PresentInfo.pNext = nullptr;
    PresentInfo.waitSemaphoreCount = 1;
    PresentInfo.pWaitSemaphores = &pImpl->SwapImageReadySempahores[pImpl->SwapImageIdx];
    PresentInfo.swapchainCount = 1;
    PresentInfo.pSwapchains = &pImpl->Swapchain;
    PresentInfo.pImageIndices = &pImpl->SwapImageIdx;
    PresentInfo.pResults = nullptr;

    pImpl->FrameIndex += 1;
    vkQueuePresentKHR(pImpl->GraphicsQueue, &PresentInfo); // TODO(nemjit001): Handle present failure
}

uint64_t BVulkanRenderBackend::GetFrameIndex()
{
    return pImpl->FrameIndex;
}

void BVulkanRenderBackend::Execute(GPUCommandQueue CommandQueue, uint32_t CommandListCount, IGPUCommandList** CommandLists)
{
    // Get command buffer handles for submission
    std::vector<VkCommandBufferSubmitInfo> CommandBufferInfos;
    CommandBufferInfos.reserve(CommandListCount);
    for (uint32_t i = 0; i < CommandListCount; i++)
    {
        if (CommandLists[i] == nullptr) {
            continue;
        }

        // This down cast can be asumed safe since only a single renderer should be active at a given time.
        BVulkanCommandList const* VulkanCommandList = static_cast<BVulkanCommandList const*>(CommandLists[i]);
        BVulkanCommandListImpl const* CommandListImpl = VulkanCommandList->GetImpl();

        VkCommandBufferSubmitInfo CommandBufferInfo{};
        CommandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        CommandBufferInfo.pNext = nullptr;
        CommandBufferInfo.commandBuffer = CommandListImpl->CommandBuffer;
        CommandBufferInfo.deviceMask = 0;

        CommandBufferInfos.push_back(CommandBufferInfo);
    }

    // Select correct command queue & sync state for submission
    VkQueue TargetQueue = VK_NULL_HANDLE;
    VkFence TargetQueueFence = VK_NULL_HANDLE;
    if (CommandQueue == GPUCommandQueue_Direct)
    {
        TargetQueue = pImpl->GraphicsQueue;
        TargetQueueFence = pImpl->GraphicsQueueFence;
    }
    else if (CommandQueue == GPUCommandQueue_Compute)
    {
        TargetQueue = pImpl->ComputeQueue;
        TargetQueueFence = pImpl->ComputeQueueFence;
    }
    else if (CommandQueue == GPUCommandQueue_Transfer)
    {
        TargetQueue = pImpl->TransferQueue;
        TargetQueueFence = pImpl->TransferQueueFence;
    }

    // Wait on previous submission
    // FIXME(nemjit001): This is suboptimal synchronization, but for the general case it should work fine
    vkWaitForFences(pImpl->Device, 1, &TargetQueueFence, VK_TRUE, UINT64_MAX);
    vkResetFences(pImpl->Device, 1, &TargetQueueFence);

    // Submit commands :)
    VkSubmitInfo2 SubmitInfo{};
    SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    SubmitInfo.pNext = nullptr;
    SubmitInfo.waitSemaphoreInfoCount = 0;
    SubmitInfo.pWaitSemaphoreInfos = nullptr;
    SubmitInfo.commandBufferInfoCount = static_cast<uint32_t>(CommandBufferInfos.size());
    SubmitInfo.pCommandBufferInfos = CommandBufferInfos.data();
    SubmitInfo.signalSemaphoreInfoCount = 0;
    SubmitInfo.pSignalSemaphoreInfos = nullptr;

    vkQueueSubmit2(TargetQueue, 1, &SubmitInfo, TargetQueueFence);
}

IGPUCommandList* BVulkanRenderBackend::CreateCommandList()
{
    return new BVulkanCommandList(pImpl.get());
}

void BVulkanRenderBackend::DestroyCommandList(IGPUCommandList* CommandList)
{
    delete CommandList;
}

BGPUBufferHandle BVulkanRenderBackend::CreateBuffer(GPUHeapType HeapType, BGPUBufferDesc BufferDesc)
{
    VkBufferCreateInfo BufferCreateInfo{};
    BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    BufferCreateInfo.pNext = nullptr;
    BufferCreateInfo.flags = 0; // TODO(nemjit001): Check if specifying flags is needed
    BufferCreateInfo.size = BufferDesc.Size;
    BufferCreateInfo.usage = static_cast<VkBufferUsageFlags>(BufferDesc.BufferUsage);
    BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    BufferCreateInfo.queueFamilyIndexCount = 0;
    BufferCreateInfo.pQueueFamilyIndices = nullptr;

    VmaAllocationCreateInfo AllocationCreateInfo{};
    AllocationCreateInfo.flags = 0;
    AllocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
    AllocationCreateInfo.preferredFlags = GPUHeapTypeLookupTable[HeapType];
    AllocationCreateInfo.requiredFlags = 0;
    AllocationCreateInfo.memoryTypeBits = 0;
    AllocationCreateInfo.pool = VK_NULL_HANDLE;
    AllocationCreateInfo.pUserData = nullptr;

    VkBuffer Buffer = VK_NULL_HANDLE;
    VmaAllocation Allocation = VK_NULL_HANDLE;
    if (vmaCreateBuffer(pImpl->Allocator, &BufferCreateInfo, &AllocationCreateInfo, &Buffer, &Allocation, nullptr) != VK_SUCCESS) {
        return TRenderResourcePool<BGPUBuffer>::CreateInvalidHandle();
    }

    BGPUBuffer const NewBuffer{ Buffer, Allocation };
    return pImpl->BufferPool.Allocate(NewBuffer);
}

void BVulkanRenderBackend::DestroyBuffer(BGPUBufferHandle Buffer)
{
    BGPUBuffer BufferResource{};
    if (!pImpl->BufferPool.GetResource(Buffer, BufferResource)) {
        return;
    }

    vmaDestroyBuffer(pImpl->Allocator, BufferResource.Buffer, BufferResource.Allocation);
    pImpl->BufferPool.Free(Buffer);
}

BGPUTextureHandle BVulkanRenderBackend::CreateTexture(GPUHeapType HeapType, BGPUTextureDesc TextureDesc)
{
    // Set correct image type
    VkImageType ImageType = GPUTextureTypeLookupTable[TextureDesc.TextureType];
    VkFormat ImageFormat = GPUFormatLookupTable[TextureDesc.TextureFormat];

    // Set up correct image layers and depth
    VkExtent3D ImageExtent = VkExtent3D{ TextureDesc.Width, TextureDesc.Height, TextureDesc.DepthOrLayers };
    uint32_t ImageArrayLayers = 1;
    if (ImageType != VK_IMAGE_TYPE_3D) {
        ImageExtent.depth = 1;
        ImageArrayLayers = TextureDesc.DepthOrLayers;
    }

    // Create image
    VkImageCreateInfo ImageCreateInfo{};
    ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    ImageCreateInfo.pNext = nullptr;
    ImageCreateInfo.flags = 0; // TODO(nemjit001): Check if specifying flags is needed
    ImageCreateInfo.imageType = ImageType;
    ImageCreateInfo.format = ImageFormat;
    ImageCreateInfo.extent = ImageExtent;
    ImageCreateInfo.mipLevels = TextureDesc.MipLevels;
    ImageCreateInfo.arrayLayers = ImageArrayLayers;
    ImageCreateInfo.samples = static_cast<VkSampleCountFlagBits>(TextureDesc.SampleCount);
    ImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL; // Texture tiling should (almost) always be set to optimal, staging buffers will be used for populating texture memory.
    ImageCreateInfo.usage = static_cast<VkImageUsageFlags>(TextureDesc.TextureUsage);
    ImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    ImageCreateInfo.queueFamilyIndexCount = 0;
    ImageCreateInfo.pQueueFamilyIndices = nullptr;
    ImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VmaAllocationCreateInfo AllocationCreateInfo{};
    AllocationCreateInfo.flags = 0;
    AllocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
    AllocationCreateInfo.preferredFlags = GPUHeapTypeLookupTable[HeapType];
    AllocationCreateInfo.requiredFlags = 0;
    AllocationCreateInfo.memoryTypeBits = 0;
    AllocationCreateInfo.pool = VK_NULL_HANDLE;
    AllocationCreateInfo.pUserData = nullptr;

    VkImage Image = VK_NULL_HANDLE;
    VmaAllocation Allocation = VK_NULL_HANDLE;
    if (vmaCreateImage(pImpl->Allocator, &ImageCreateInfo, &AllocationCreateInfo, &Image, &Allocation, nullptr) != VK_SUCCESS) {
        return TRenderResourcePool<BGPUTexture>::CreateInvalidHandle();
    }

    BGPUTexture const NewTexture{ Image, Allocation };
    return pImpl->TexturePool.Allocate(NewTexture);
}

void BVulkanRenderBackend::DestroyTexture(BGPUTextureHandle Texture)
{
    BGPUTexture TextureResource{};
    if (!pImpl->TexturePool.GetResource(Texture, TextureResource)) {
        return;
    }

    vmaDestroyImage(pImpl->Allocator, TextureResource.Image, TextureResource.Allocation);
    pImpl->TexturePool.Free(Texture);
}

BGPUPipelineStateHandle BVulkanRenderBackend::CreateComputePipelineState(BGPUComputePipelineStateDesc PipelineDesc)
{
    // Compile shader source into bytecode
    CComPtr<IDxcBlob> ComputeShaderBytecode;
    if (!CompileShader(PipelineDesc.ComputeShader, &ComputeShaderBytecode)) {
        return TRenderResourcePool<BGPUPipelineState>::CreateInvalidHandle();
    }

    CompiledShader ComputeShader{};
    ComputeShader.Stage = PipelineDesc.ComputeShader.Stage;
    ComputeShader.Bytecode = ComputeShaderBytecode;

    // Reflect & create shader pipeline layout
    std::vector<CompiledShader> const CompiledShaders = { ComputeShader };
    std::vector<BGPUPipelineLocalSize> LocalSizes;
    std::vector<VkPushConstantRange> PushConstantRanges;
    std::vector<VkDescriptorSetLayout> DescriptorSetLayouts;
    if (!ReflectPipelineLayout(pImpl->Device, CompiledShaders, LocalSizes, PushConstantRanges, DescriptorSetLayouts))
    {
        for (auto const& Layout : DescriptorSetLayouts) {
            vkDestroyDescriptorSetLayout(pImpl->Device, Layout, nullptr);
        }

        return TRenderResourcePool<BGPUPipelineState>::CreateInvalidHandle();
    }

    VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo{};
    PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    PipelineLayoutCreateInfo.pNext = nullptr;
    PipelineLayoutCreateInfo.flags = 0;
    PipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(DescriptorSetLayouts.size());
    PipelineLayoutCreateInfo.pSetLayouts = DescriptorSetLayouts.data();
    PipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(PushConstantRanges.size());
    PipelineLayoutCreateInfo.pPushConstantRanges = PushConstantRanges.data();

    VkPipelineLayout PipelineLayout = VK_NULL_HANDLE;
    if (vkCreatePipelineLayout(pImpl->Device, &PipelineLayoutCreateInfo, nullptr, &PipelineLayout) != VK_SUCCESS)
    {
        for (auto const& Layout : DescriptorSetLayouts) {
            vkDestroyDescriptorSetLayout(pImpl->Device, Layout, nullptr);
        }

        return TRenderResourcePool<BGPUPipelineState>::CreateInvalidHandle();
    }
    
    // Create shader module from source blob
    VkShaderModule ComputeShaderModule = CreateShaderModule(pImpl->Device, static_cast<uint32_t const*>(ComputeShaderBytecode->GetBufferPointer()), ComputeShaderBytecode->GetBufferSize());
    if (ComputeShaderModule == VK_NULL_HANDLE)
    {
        for (auto const& Layout : DescriptorSetLayouts) {
            vkDestroyDescriptorSetLayout(pImpl->Device, Layout, nullptr);
        }
        vkDestroyPipelineLayout(pImpl->Device, PipelineLayout, nullptr);

        return TRenderResourcePool<BGPUPipelineState>::CreateInvalidHandle();
    }

    // Create pipeline state
    VkPipelineShaderStageCreateInfo ComputeStageCreateInfo{};
    ComputeStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    ComputeStageCreateInfo.pName = nullptr;
    ComputeStageCreateInfo.flags = 0;
    ComputeStageCreateInfo.stage = static_cast<VkShaderStageFlagBits>(PipelineDesc.ComputeShader.Stage);
    ComputeStageCreateInfo.module = ComputeShaderModule;
    ComputeStageCreateInfo.pName = PipelineDesc.ComputeShader.Entrypoint;
    ComputeStageCreateInfo.pSpecializationInfo = nullptr;

    VkComputePipelineCreateInfo ComputePipelineCreateInfo{};
    ComputePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    ComputePipelineCreateInfo.pNext = nullptr;
    ComputePipelineCreateInfo.flags = 0;
    ComputePipelineCreateInfo.stage = ComputeStageCreateInfo;
    ComputePipelineCreateInfo.layout = PipelineLayout;
    ComputePipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    ComputePipelineCreateInfo.basePipelineIndex = 0;

    VkPipeline ComputePipeline = VK_NULL_HANDLE;
    if (vkCreateComputePipelines(pImpl->Device, VK_NULL_HANDLE, 1, &ComputePipelineCreateInfo, nullptr, &ComputePipeline) != VK_SUCCESS)
    {
        for (auto const& Layout : DescriptorSetLayouts) {
            vkDestroyDescriptorSetLayout(pImpl->Device, Layout, nullptr);
        }
        vkDestroyPipelineLayout(pImpl->Device, PipelineLayout, nullptr);
        vkDestroyShaderModule(pImpl->Device, ComputeShaderModule, nullptr);
        return TRenderResourcePool<BGPUPipelineState>::CreateInvalidHandle();
    }

    vkDestroyShaderModule(pImpl->Device, ComputeShaderModule, nullptr);
    BGPUPipelineLocalSize PipelineDispatchSize = LocalSizes[0]; // always using 1st dispatch size
    BGPUPipelineState const NewPSO{ VK_PIPELINE_BIND_POINT_COMPUTE, PipelineLayout, ComputePipeline, PipelineDispatchSize, DescriptorSetLayouts };
    return pImpl->PSOPool.Allocate(NewPSO);
}

BGPUPipelineStateHandle BVulkanRenderBackend::CreateRenderPipelineState(BGPURenderPipelineStateDesc PipelineDesc)
{
    return TRenderResourcePool<BGPUPipelineState>::CreateInvalidHandle();
}

void BVulkanRenderBackend::DestroyPipelineState(BGPUPipelineStateHandle PipelineState)
{
    BGPUPipelineState PSOResource{};
    if (!pImpl->PSOPool.GetResource(PipelineState, PSOResource)) {
        return;
    }

    for (auto const& Layout : PSOResource.DescriptorSetLayouts) {
        vkDestroyDescriptorSetLayout(pImpl->Device, Layout, nullptr);
    }

    vkDestroyPipeline(pImpl->Device, PSOResource.Pipeline, nullptr);
    vkDestroyPipelineLayout(pImpl->Device, PSOResource.PipelineLayout, nullptr);
    pImpl->PSOPool.Free(PipelineState);
}

BGPUPipelineLocalSize BVulkanRenderBackend::GetPipelineLocalSize(BGPUPipelineStateHandle PipelineState)
{
    BGPUPipelineState PSOResource{};
    if (!pImpl->PSOPool.GetResource(PipelineState, PSOResource)) {
        return {};
    }

    return PSOResource.LocalSize;
}

#endif //BONSAI_VULKAN_SUPPORT
