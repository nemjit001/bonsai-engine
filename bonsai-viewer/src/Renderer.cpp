#include "Renderer.hpp"

#define VOLK_IMPLEMENTATION

#include <algorithm>
#include <cstdio>
#include <vector>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <volk.h>

#define BONSAI_VULKAN_API_VERSION   VK_API_VERSION_1_3

#ifndef NDEBUG
static constexpr bool RendererDebugEnabled = true;
#else
static constexpr bool RendererDebugEnabled = false;
#endif //!NDEBUG

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

/// @brief Renderer backend implementation.
struct BRenderer::Impl
{
    SDL_Window* Window;
    VkInstance Instance;
    VkDebugUtilsMessengerEXT DebugMessenger;
    VkSurfaceKHR Surface;
    VkPhysicalDevice PhysicalDevice;
    VkDevice Device;
    QueueFamilyInfo DeviceQueueFamilies;
    VkQueue GraphicsQueue;
    VkQueue ComputeQueue;
    VkQueue TransferQueue;
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
    fprintf(stderr, "[Vulkan] %s\n", CallbackData->pMessage);
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

        if (!PhysicalDeviceHasSurfaceSupport(PhysicalDevice, DefaultSurface)) {
            continue;
        }

        return PhysicalDevice;
    }

    return VK_NULL_HANDLE;
}

uint32_t FindQueueFamily(
    std::vector<VkQueueFamilyProperties> const& QueueFamilies,
    VkPhysicalDevice PhysicalDevice,
    VkSurfaceKHR DefaultSurface,
    VkQueueFlags RequiredFlags,
    VkQueueFlags IgnoredFlags
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
            && (QueueFamilies[i].queueFlags & IgnoredFlags) == 0
        ) {
            return i;
        }
    }

    return VK_QUEUE_FAMILY_IGNORED;
}

bool GetDeviceQueueFamilies(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR DefaultSurface, QueueFamilyInfo& OutQueueFamilies)
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

std::vector<uint32_t> GetUniqueQueueFamilies(QueueFamilyInfo const& QueueFamilies)
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

BRenderer::BRenderer()
    :
    pImpl(std::make_unique<Impl>())
{
    //
}

BRenderer::~BRenderer() = default;

RendererResult BRenderer::Init()
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
    std::vector<char const*> DeviceExtensionNames = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

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
    std::vector<VkDeviceQueueCreateInfo> QueueCreateInfos{}; QueueCreateInfos.reserve(QueueFamilyIndices.size());
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

    // Get queue families from device
    VkQueue GraphicsQueue = VK_NULL_HANDLE;
    VkQueue ComputeQueue = VK_NULL_HANDLE;
    VkQueue TransferQueue = VK_NULL_HANDLE;
    vkGetDeviceQueue(Device, DeviceQueueFamilies.GraphicsFamily, 0, &GraphicsQueue);
    vkGetDeviceQueue(Device, DeviceQueueFamilies.ComputeFamily, 0, &ComputeQueue);
    vkGetDeviceQueue(Device, DeviceQueueFamilies.TransferFamily, 0, &TransferQueue);

    pImpl->Window = Window;
    pImpl->Instance = Instance;
    pImpl->DebugMessenger = DebugMessenger;
    pImpl->Surface = Surface;
    pImpl->PhysicalDevice = PhysicalDevice;
    pImpl->Device = Device;
    pImpl->DeviceQueueFamilies = DeviceQueueFamilies;
    pImpl->GraphicsQueue = GraphicsQueue;
    pImpl->ComputeQueue = ComputeQueue;
    pImpl->TransferQueue = TransferQueue;
    return RENDERER_RESULT_OK;
}

void BRenderer::Shutdown()
{
    vkDestroyDevice(pImpl->Device, nullptr);

    vkDestroySurfaceKHR(pImpl->Instance, pImpl->Surface, nullptr);
    if constexpr (RendererDebugEnabled) {
        vkDestroyDebugUtilsMessengerEXT(pImpl->Instance, pImpl->DebugMessenger, nullptr);
    }
    vkDestroyInstance(pImpl->Instance, nullptr);

    SDL_DestroyWindow(pImpl->Window);
    volkFinalize();
}

void BRenderer::Frame()
{
    //
}
