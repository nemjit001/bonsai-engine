#include "Renderer.hpp"

#define VOLK_IMPLEMENTATION

#include <cstdio>
#include <vector>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <volk.h>

struct BRenderer::Impl
{
    SDL_Window* Window;
    VkInstance Instance;
    VkDebugUtilsMessengerEXT DebugMessenger;
    VkSurfaceKHR Surface;
};

static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT Severity,
    VkDebugUtilsMessageTypeFlagsEXT Type,
    VkDebugUtilsMessengerCallbackDataEXT const* CallbackData,
    void* UserData
)
{
    fprintf(stderr, "[Vulkan] %s\n", CallbackData->pMessage);
    return VK_FALSE;
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

    VkApplicationInfo AppInfo{};
    AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    AppInfo.pNext = nullptr;
    AppInfo.pApplicationName = "Bonsai Viewer";
    AppInfo.applicationVersion = 0;
    AppInfo.pEngineName = "Bonsai Viewer";
    AppInfo.engineVersion = 0;
    AppInfo.apiVersion = VK_API_VERSION_1_3;

    uint32_t WindowExtensionCount = 0;
    char const** WindowExtensionNames = const_cast<char const**>(SDL_Vulkan_GetInstanceExtensions(&WindowExtensionCount));
    std::vector<char const*> InstanceExtensionNames(WindowExtensionNames, WindowExtensionNames + WindowExtensionCount);
    InstanceExtensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

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
        | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    DebugMessengerCreateInfo.pfnUserCallback = VulkanDebugCallback;
    DebugMessengerCreateInfo.pUserData = nullptr;

    VkInstanceCreateInfo InstanceCreateInfo{};
    InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    InstanceCreateInfo.pNext = &DebugMessengerCreateInfo;
    InstanceCreateInfo.flags = 0;
    InstanceCreateInfo.pApplicationInfo = &AppInfo;
    InstanceCreateInfo.enabledLayerCount = 0;
    InstanceCreateInfo.ppEnabledLayerNames = nullptr;
    InstanceCreateInfo.enabledExtensionCount = InstanceExtensionNames.size();
    InstanceCreateInfo.ppEnabledExtensionNames = InstanceExtensionNames.data();

    VkInstance Instance = VK_NULL_HANDLE;
    if (vkCreateInstance(&InstanceCreateInfo, nullptr, &Instance) != VK_SUCCESS) {
        return RENDERER_RESULT_GFX_API_BAD_INIT;
    }
    volkLoadInstance(Instance);

    VkDebugUtilsMessengerEXT DebugMessenger = VK_NULL_HANDLE;
    if (vkCreateDebugUtilsMessengerEXT(Instance, &DebugMessengerCreateInfo, nullptr, &DebugMessenger) != VK_SUCCESS) {
        return RENDERER_RESULT_GFX_API_BAD_INIT;
    }

    VkSurfaceKHR Surface = VK_NULL_HANDLE;
    if (!SDL_Vulkan_CreateSurface(Window, Instance, nullptr, &Surface)) {
        return RENDERER_RESULT_GFX_API_BAD_INIT;
    }

    pImpl->Window = Window;
    pImpl->Instance = Instance;
    pImpl->DebugMessenger = DebugMessenger;
    pImpl->Surface = Surface;
    return RENDERER_RESULT_OK;
}

void BRenderer::Shutdown()
{
    vkDestroySurfaceKHR(pImpl->Instance, pImpl->Surface, nullptr);
    vkDestroyDebugUtilsMessengerEXT(pImpl->Instance, pImpl->DebugMessenger, nullptr);
    vkDestroyInstance(pImpl->Instance, nullptr);

    SDL_DestroyWindow(pImpl->Window);
    volkFinalize();
}

void BRenderer::Frame()
{
    //
}
