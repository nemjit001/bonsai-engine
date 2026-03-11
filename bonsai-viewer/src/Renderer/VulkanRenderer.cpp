#include "VulkanRenderer.hpp"

#define VMA_IMPLEMENTATION
#define VOLK_IMPLEMENTATION

#include <algorithm>
#include <array>
#include <cstdio>
#include <vector>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <volk.h>
#include <vk_mem_alloc.h>
#include "Logging.hpp"
#include "Vulkan/VulkanTypes.hpp"

#define BONSAI_VULKAN_API_VERSION   VK_API_VERSION_1_3
#define BONSAI_FRAMES_IN_FLIGHT     2

#ifndef NDEBUG
static constexpr bool RendererDebugEnabled = true;
#else
static constexpr bool RendererDebugEnabled = false;
#endif //!NDEBUG

/// @brief Device extension names that are enabled by default.
static constexpr std::array DefaultDeviceExtensionNames = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,
};

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

/// @brief Vulkan renderer backend implementation.
struct BVulkanRendererImpl
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
    std::vector<VkSemaphore> SwapImageAvailable;        // Sized on frames in flight
    std::vector<VkFence> FrameReady;                    // Sized on frames in flight
    std::vector<VkSemaphore> RenderingFinished;         // Sized on swap image count

    /** Frame state */
    std::vector<VkCommandPool> GraphicsCmdPools;        // Sized on frames in flight
    std::vector<VkCommandBuffer> FrameCommandBuffers;   // Sized on frames in flight
    uint64_t FrameIndex;

    /** Resource pools */
    TRenderResourcePool<BGPUBuffer> BufferPool;
    TRenderResourcePool<BGPUTexture> TexturePool;
};

/// @brief Convert a GPUFormat to a VkFormat value.
/// @param format 
/// @return 
static VkFormat ConvertToVkFormat(GPUFormat format)
{
    switch (format)
    {
    case GPUFormat_Unknown:
        return VK_FORMAT_UNDEFINED;
    case GPUFormat_R8_UNORM:
        return VK_FORMAT_R8_UNORM;
    case GPUFormat_R8_SNORM:
        return VK_FORMAT_R8_SNORM;
    case GPUFormat_R8_UINT:
        return VK_FORMAT_R8_UINT;
    case GPUFormat_R8_SINT:
        return VK_FORMAT_R8_SINT;
    case GPUFormat_RG8_UNORM:
        return VK_FORMAT_R8G8_UNORM;
    case GPUFormat_RG8_SNORM:
        return VK_FORMAT_R8G8_SNORM;
    case GPUFormat_RG8_UINT:
        return VK_FORMAT_R8G8_UINT;
    case GPUFormat_RG8_SINT:
        return VK_FORMAT_R8G8_SINT;
    case GPUFormat_BGR8_UNORM:
        return VK_FORMAT_B8G8R8_UNORM;
    case GPUFormat_BGR8_SNORM:
        return VK_FORMAT_B8G8R8_SNORM;
    case GPUFormat_BGR8_UINT:
        return VK_FORMAT_B8G8R8_UINT;
    case GPUFormat_BGR8_SINT:
        return VK_FORMAT_B8G8R8_SINT;
    case GPUFormat_BGR8_SRGB:
        return VK_FORMAT_B8G8R8_SRGB;
    case GPUFormat_RGBA8_UNORM:
        return VK_FORMAT_R8G8B8A8_UNORM;
    case GPUFormat_RGBA8_SNORM:
        return VK_FORMAT_R8G8B8A8_SNORM;
    case GPUFormat_RGBA8_UINT:
        return VK_FORMAT_R8G8B8A8_UINT;
    case GPUFormat_RGBA8_SINT:
        return VK_FORMAT_R8G8B8A8_SINT;
    case GPUFormat_RGBA8_SRGB:
        return VK_FORMAT_R8G8B8A8_SRGB;
    case GPUFormat_BGRA8_UNORM:
        return VK_FORMAT_B8G8R8A8_UNORM;
    case GPUFormat_BGRA8_SNORM:
        return VK_FORMAT_B8G8R8A8_SNORM;
    case GPUFormat_BGRA8_UINT:
        return VK_FORMAT_B8G8R8A8_UINT;
    case GPUFormat_BGRA8_SINT:
        return VK_FORMAT_B8G8R8A8_SINT;
    case GPUFormat_BGRA8_SRGB:
        return VK_FORMAT_B8G8R8A8_SRGB;
    case GPUFormat_R16_UNORM:
        return VK_FORMAT_R16_UNORM;
    case GPUFormat_R16_SNORM:
        return VK_FORMAT_R16_SNORM;
    case GPUFormat_R16_UINT:
        return VK_FORMAT_R16_UINT;
    case GPUFormat_R16_SINT:
        return VK_FORMAT_R16_SINT;
    case GPUFormat_R16_SFLOAT:
        return VK_FORMAT_R16_SFLOAT;
    case GPUFormat_RG16_UNORM:
        return VK_FORMAT_R16G16_UNORM;
    case GPUFormat_RG16_SNORM:
        return VK_FORMAT_R16G16_SNORM;
    case GPUFormat_RG16_UINT:
        return VK_FORMAT_R16G16_UINT;
    case GPUFormat_RG16_SINT:
        return VK_FORMAT_R16G16_SINT;
    case GPUFormat_RG16_SFLOAT:
        return VK_FORMAT_R16G16_SFLOAT;
    case GPUFormat_RGB16_UNORM:
        return VK_FORMAT_R16G16B16_UNORM;
    case GPUFormat_RGB16_SNORM:
        return VK_FORMAT_R16G16B16_SNORM;
    case GPUFormat_RGB16_UINT:
        return VK_FORMAT_R16G16B16_UINT;
    case GPUFormat_RGB16_SINT:
        return VK_FORMAT_R16G16B16_SINT;
    case GPUFormat_RGB16_SFLOAT:
        return VK_FORMAT_R16G16B16_SFLOAT;
    case GPUFormat_RGBA16_UNORM:
        return VK_FORMAT_R16G16B16A16_UNORM;
    case GPUFormat_RGBA16_SNORM:
        return VK_FORMAT_R16G16B16A16_SNORM;
    case GPUFormat_RGBA16_UINT:
        return VK_FORMAT_R16G16B16A16_UINT;
    case GPUFormat_RGBA16_SINT:
        return VK_FORMAT_R16G16B16A16_SINT;
    case GPUFormat_RGBA16_SFLOAT:
        return VK_FORMAT_R16G16B16A16_SFLOAT;
    case GPUFormat_R32_UINT:
        return VK_FORMAT_R32_UINT;
    case GPUFormat_R32_SINT:
        return VK_FORMAT_R32_SINT;
    case GPUFormat_R32_SFLOAT:
        return VK_FORMAT_R32_SFLOAT;
    case GPUFormat_RG32_UINT:
        return VK_FORMAT_R32G32_UINT;
    case GPUFormat_RG32_SINT:
        return VK_FORMAT_R32G32_SINT;
    case GPUFormat_RG32_SFLOAT:
        return VK_FORMAT_R32G32_SFLOAT;
    case GPUFormat_RGB32_UINT:
        return VK_FORMAT_R32G32B32_UINT;
    case GPUFormat_RGB32_SINT:
        return VK_FORMAT_R32G32B32_SINT;
    case GPUFormat_RGB32_SFLOAT:
        return VK_FORMAT_R32G32B32_SFLOAT;
    case GPUFormat_RGBA32_UINT:
        return VK_FORMAT_R32G32B32A32_UINT;
    case GPUFormat_RGBA32_SINT:
        return VK_FORMAT_R32G32B32A32_SINT;
    case GPUFormat_RGBA32_SFLOAT:
        return VK_FORMAT_R32G32B32A32_SFLOAT;
    case GPUFormat_D16_UNORM:
        return VK_FORMAT_D16_UNORM;
    case GPUFormat_D32_SFLOAT:
        return VK_FORMAT_D32_SFLOAT;
    case GPUFormat_S8_UINT:
        return VK_FORMAT_S8_UINT;
    case GPUFormat_D16_UNORM_S8_UINT:
        return VK_FORMAT_D16_UNORM_S8_UINT;
    case GPUFormat_D24_UNORM_S8_UINT:
        return VK_FORMAT_D24_UNORM_S8_UINT;
    case GPUFormat_D32_SFLOAT_S8_UINT:
        return VK_FORMAT_D32_SFLOAT_S8_UINT;
    default:
        break;
    }

    return VK_FORMAT_UNDEFINED;
}

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
            if (strcmp(AvailableExtension.extensionName, RequestedExtensionName) == 0)
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
            if (strcmp(AvailableExtension.extensionName, RequestedExtensionName) == 0)
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
        // FIXME(nemjit001): Have fallback queue setup for single queue physical devices
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

static void RecordRenderCommands(BVulkanRendererImpl const* RendererImpl, uint32_t SwapImageIndex, VkCommandBuffer FrameCommandBuffer)
{
    VkCommandBufferBeginInfo FrameCommandsBeginInfo{};
    FrameCommandsBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    FrameCommandsBeginInfo.pNext = nullptr;
    FrameCommandsBeginInfo.flags = 0;
    FrameCommandsBeginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(FrameCommandBuffer, &FrameCommandsBeginInfo) != VK_SUCCESS) {
        return;
    }

    VkImageMemoryBarrier2 SwapPresentBarrier{};
    SwapPresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    SwapPresentBarrier.pNext = nullptr;
    SwapPresentBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    SwapPresentBarrier.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
    SwapPresentBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    SwapPresentBarrier.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT;
    SwapPresentBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    SwapPresentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    SwapPresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    SwapPresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    SwapPresentBarrier.image = RendererImpl->SwapchainImages[SwapImageIndex];
    SwapPresentBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    SwapPresentBarrier.subresourceRange.baseMipLevel = 0;
    SwapPresentBarrier.subresourceRange.levelCount = 1;
    SwapPresentBarrier.subresourceRange.baseArrayLayer = 0;
    SwapPresentBarrier.subresourceRange.layerCount = 1;

    VkDependencyInfo PresentDependency{};
    PresentDependency.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    PresentDependency.pNext = nullptr;
    PresentDependency.dependencyFlags = 0;
    PresentDependency.imageMemoryBarrierCount = 1;
    PresentDependency.pImageMemoryBarriers = &SwapPresentBarrier;
    vkCmdPipelineBarrier2(FrameCommandBuffer, &PresentDependency);

    if (vkEndCommandBuffer(FrameCommandBuffer) != VK_SUCCESS) {
        return;
    }
}

BVulkanRenderer::BVulkanRenderer()
    :
    pImpl(std::make_unique<BVulkanRendererImpl>())
{
    //
}

BVulkanRenderer::~BVulkanRenderer() = default;

RendererResult BVulkanRenderer::Init()
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
    std::vector<char const*> InstanceExtensionNames(WindowExtensionNames, WindowExtensionNames + WindowExtensionCount);

    [[maybe_unused]]
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

    if constexpr (RendererDebugEnabled) { // Enable the debug utils extension when debug is enabled
        InstanceExtensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    if (!InstanceExtensionsAvailable(InstanceExtensionNames)) {
        return RENDERER_RESULT_GFX_API_BAD_INIT;
    }

    VkInstanceCreateInfo InstanceCreateInfo{};
    InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    InstanceCreateInfo.pNext = nullptr;
    InstanceCreateInfo.flags = 0;
    InstanceCreateInfo.pApplicationInfo = &AppInfo;
    InstanceCreateInfo.enabledLayerCount = 0;
    InstanceCreateInfo.ppEnabledLayerNames = nullptr;
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

    // Create sync primitives
    std::vector<VkSemaphore> SwapImageAvailable{};
    std::vector<VkFence> FrameReady{};
    SwapImageAvailable.reserve(BONSAI_FRAMES_IN_FLIGHT);
    FrameReady.reserve(BONSAI_FRAMES_IN_FLIGHT);
    for (uint32_t i = 0; i < BONSAI_FRAMES_IN_FLIGHT; i++)
    {
        VkSemaphoreCreateInfo SemaphoreCreateInfo{};
        SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        SemaphoreCreateInfo.pNext = nullptr;
        SemaphoreCreateInfo.flags = 0;

        VkSemaphore Semaphore = VK_NULL_HANDLE;
        if (vkCreateSemaphore(Device, &SemaphoreCreateInfo, nullptr, &Semaphore) != VK_SUCCESS) {
            return RENDERER_RESULT_FRAME_STATE_BAD_INIT;
        }
        SwapImageAvailable.push_back(Semaphore);

        VkFenceCreateInfo FenceCreateInfo{};
        FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        FenceCreateInfo.pNext = nullptr;
        FenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        VkFence Fence = VK_NULL_HANDLE;
        if (vkCreateFence(Device, &FenceCreateInfo, nullptr, &Fence)) {
            return RENDERER_RESULT_FRAME_STATE_BAD_INIT;
        }
        FrameReady.push_back(Fence);
    }

    std::vector<VkSemaphore> RenderingFinished{};
    RenderingFinished.reserve(SwapImageCount);
    for (uint32_t i = 0; i < SwapImageCount; i++)
    {
        VkSemaphoreCreateInfo SemaphoreCreateInfo{};
        SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        SemaphoreCreateInfo.pNext = nullptr;
        SemaphoreCreateInfo.flags = 0;

        VkSemaphore Semaphore = VK_NULL_HANDLE;
        if (vkCreateSemaphore(Device, &SemaphoreCreateInfo, nullptr, &Semaphore) != VK_SUCCESS) {
            return RENDERER_RESULT_FRAME_STATE_BAD_INIT;
        }
        RenderingFinished.push_back(Semaphore);
    }

    // Create frame state
    std::vector<VkCommandPool> GraphicsCmdPools{};
    GraphicsCmdPools.reserve(BONSAI_FRAMES_IN_FLIGHT);
    for (uint32_t i = 0; i < BONSAI_FRAMES_IN_FLIGHT; i++)
    {
        VkCommandPoolCreateInfo GraphicsPoolCreateInfo{};
        GraphicsPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        GraphicsPoolCreateInfo.pNext = nullptr;
        GraphicsPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        GraphicsPoolCreateInfo.queueFamilyIndex = DeviceQueueFamilies.GraphicsFamily;

        VkCommandPool GraphicsPool = VK_NULL_HANDLE;
        if (vkCreateCommandPool(Device, &GraphicsPoolCreateInfo, nullptr, &GraphicsPool) != VK_SUCCESS) {
            return RENDERER_RESULT_FRAME_STATE_BAD_INIT;
        }
        GraphicsCmdPools.push_back(GraphicsPool);
    }

    std::vector<VkCommandBuffer> FrameCommandBuffers{};
    FrameCommandBuffers.reserve(BONSAI_FRAMES_IN_FLIGHT);
    for (uint32_t i = 0; i < BONSAI_FRAMES_IN_FLIGHT; i++)
    {
        VkCommandBufferAllocateInfo FrameCommandAllocateInfo{};
        FrameCommandAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        FrameCommandAllocateInfo.pNext = nullptr;
        FrameCommandAllocateInfo.commandPool = GraphicsCmdPools[i];
        FrameCommandAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        FrameCommandAllocateInfo.commandBufferCount = 1;

        VkCommandBuffer FrameCommandBuffer = VK_NULL_HANDLE;
        if (vkAllocateCommandBuffers(Device, &FrameCommandAllocateInfo, &FrameCommandBuffer) != VK_SUCCESS) {
            return RENDERER_RESULT_FRAME_STATE_BAD_INIT;
        }
        FrameCommandBuffers.push_back(FrameCommandBuffer);
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
    pImpl->SwapImageAvailable = SwapImageAvailable;
    pImpl->FrameReady = FrameReady;
    pImpl->RenderingFinished = RenderingFinished;
    pImpl->GraphicsCmdPools = GraphicsCmdPools;
    pImpl->FrameCommandBuffers = FrameCommandBuffers;
    pImpl->FrameIndex = 0;
    return RENDERER_RESULT_OK;
}

void BVulkanRenderer::Shutdown()
{
    // Ensure any work on the GPU finishes before cleanup starts
    vkDeviceWaitIdle(pImpl->Device);

    // Clean up frame state
    for (auto const& GraphicsCmdPool : pImpl->GraphicsCmdPools) {
        vkDestroyCommandPool(pImpl->Device, GraphicsCmdPool, nullptr);
    }

    // Clean up sync state
    for (auto const& Semaphore : pImpl->RenderingFinished) {
        vkDestroySemaphore(pImpl->Device, Semaphore, nullptr);
    }

    for (uint32_t i = 0; i < BONSAI_FRAMES_IN_FLIGHT; i++) {
        vkDestroyFence(pImpl->Device, pImpl->FrameReady[i], nullptr);
        vkDestroySemaphore(pImpl->Device, pImpl->SwapImageAvailable[i], nullptr);
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

void BVulkanRenderer::Frame()
{
    // Acquire next frame
    uint32_t const FrameInFlight = pImpl->FrameIndex % BONSAI_FRAMES_IN_FLIGHT;
    vkWaitForFences(pImpl->Device, 1, &pImpl->FrameReady[FrameInFlight], VK_TRUE, UINT64_MAX);

    uint32_t SwapImageIndex = 0;
    VkResult const AcquireResult = vkAcquireNextImageKHR(pImpl->Device, pImpl->Swapchain, UINT64_MAX, pImpl->SwapImageAvailable[FrameInFlight], VK_NULL_HANDLE, &SwapImageIndex);
    if (AcquireResult != VK_SUCCESS) {
        return;
    }

    // Reset frame state
    vkResetFences(pImpl->Device, 1, &pImpl->FrameReady[FrameInFlight]);
    pImpl->FrameIndex++; // Advance frame counter after fence reset. This is done here because after this point any failure needs to use the next available sync primitives to avoid deadlocks.

    VkCommandBuffer FrameCommandBuffer = pImpl->FrameCommandBuffers[FrameInFlight];
    vkResetCommandBuffer(FrameCommandBuffer, 0 /* No flags */);

    // Record commands
    // TODO(nemjit001): Check if a separate function call is really what we want, especially with multiple render passes this may become a monster function...
    RecordRenderCommands(pImpl.get(), SwapImageIndex, FrameCommandBuffer);

    // Submit frame
    VkPipelineStageFlags WaitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    };
    VkSubmitInfo SubmitInfo{};
    SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    SubmitInfo.pNext = nullptr;
    SubmitInfo.waitSemaphoreCount = 1;
    SubmitInfo.pWaitSemaphores = &pImpl->SwapImageAvailable[FrameInFlight];
    SubmitInfo.pWaitDstStageMask = WaitStages;
    SubmitInfo.commandBufferCount = 1;
    SubmitInfo.pCommandBuffers = &FrameCommandBuffer;
    SubmitInfo.signalSemaphoreCount = 1;
    SubmitInfo.pSignalSemaphores = &pImpl->RenderingFinished[SwapImageIndex];
    if (vkQueueSubmit(pImpl->GraphicsQueue, 1, &SubmitInfo, pImpl->FrameReady[FrameInFlight]) != VK_SUCCESS) {
        return;
    }

    // Present frame
    VkPresentInfoKHR PresentInfo{};
    PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    PresentInfo.pNext = nullptr;
    PresentInfo.waitSemaphoreCount = 1;
    PresentInfo.pWaitSemaphores = &pImpl->RenderingFinished[SwapImageIndex];
    PresentInfo.swapchainCount = 1;
    PresentInfo.pSwapchains = &pImpl->Swapchain;
    PresentInfo.pImageIndices = &SwapImageIndex;
    PresentInfo.pResults = nullptr;

    VkResult const PresentResult = vkQueuePresentKHR(pImpl->GraphicsQueue, &PresentInfo);
    if (PresentResult != VK_SUCCESS) {
        return;
    }
}

TRenderHandle<BGPUBuffer> BVulkanRenderer::CreateBuffer(BGPUBufferDesc BufferDesc)
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
    AllocationCreateInfo.preferredFlags = 0;
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

void BVulkanRenderer::DestroyBuffer(TRenderHandle<BGPUBuffer> Buffer)
{
    BGPUBuffer BufferResource{};
    if (!pImpl->BufferPool.GetResource(Buffer, BufferResource)) {
        return;
    }

    vmaDestroyBuffer(pImpl->Allocator, BufferResource.Buffer, BufferResource.Allocation);
    pImpl->BufferPool.Free(Buffer);
}

TRenderHandle<BGPUTexture> BVulkanRenderer::CreateTexture(BGPUTextureDesc TextureDesc)
{
    // Set correct image type
    VkImageType ImageType = VK_IMAGE_TYPE_MAX_ENUM;
    {
        switch (TextureDesc.TextureType)
        {
        case TextureType_1D:
            ImageType = VK_IMAGE_TYPE_1D;
            break;
        case TextureType_2D:
            ImageType = VK_IMAGE_TYPE_2D;
            break;
        case TextureType_3D:
            ImageType = VK_IMAGE_TYPE_3D;
            break;        
        default:
            break;
        }
    }

    // Set up correct image layers and depth
    VkExtent3D ImageExtent = VkExtent3D{ TextureDesc.Width, TextureDesc.Height, TextureDesc.DepthOrLayers };
    uint32_t ImageArrayLayers = 1;
    {
        if (ImageType != VK_IMAGE_TYPE_3D) {
            ImageExtent.depth = 1;
            ImageArrayLayers = TextureDesc.DepthOrLayers;
        }
    }

    // Create image
    VkImageCreateInfo ImageCreateInfo{};
    ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    ImageCreateInfo.pNext = nullptr;
    ImageCreateInfo.flags = 0; // TODO(nemjit001): Check if specifying flags is needed
    ImageCreateInfo.imageType = ImageType;
    ImageCreateInfo.format = ConvertToVkFormat(TextureDesc.TextureFormat);
    ImageCreateInfo.extent = ImageExtent;
    ImageCreateInfo.mipLevels = TextureDesc.MipLevels;
    ImageCreateInfo.arrayLayers = ImageArrayLayers;
    ImageCreateInfo.samples = static_cast<VkSampleCountFlagBits>(TextureDesc.SampleCount);
    ImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL; // TODO(nemjit001): Specify texture tiling mode, this is similar to DX12's resource layout parameter
    ImageCreateInfo.usage = static_cast<VkImageUsageFlags>(TextureDesc.TextureUsage);
    ImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    ImageCreateInfo.queueFamilyIndexCount = 0;
    ImageCreateInfo.pQueueFamilyIndices = nullptr;
    ImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VmaAllocationCreateInfo AllocationCreateInfo{};
    AllocationCreateInfo.flags = 0;
    AllocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
    AllocationCreateInfo.preferredFlags = 0;
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

void BVulkanRenderer::DestroyTexture(TRenderHandle<BGPUTexture> Texture)
{
    BGPUTexture TextureResource{};
    if (!pImpl->TexturePool.GetResource(Texture, TextureResource)) {
        return;
    }

    vmaDestroyImage(pImpl->Allocator, TextureResource.Image, TextureResource.Allocation);
    pImpl->TexturePool.Free(Texture);
}
