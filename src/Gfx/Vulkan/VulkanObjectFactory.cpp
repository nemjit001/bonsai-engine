#include "VulkanObjectFactory.hpp"

#include <cassert>
#include <SDL3/SDL_vulkan.h>
#include <volk.h>
#include "Gfx/Vulkan/VulkanSwapchainHelpers.hpp"

namespace Bonsai
{
    VulkanObjectFactory::VulkanObjectFactory(
        VkInstance instance,
        VkPhysicalDevice physicalDevice,
        VkDevice device,
        VulkanSharedDeviceState* sharedDeviceState
    )
        :
        _instance(instance),
        _physicalDevice(physicalDevice),
        _device(device),
        _sharedDeviceState(sharedDeviceState)
    {
        //
    }

    VulkanObjectFactory::~VulkanObjectFactory()
    {
        //
    }

    VulkanObjectFactory* VulkanObjectFactory::TryCreate(
        VkInstance instance,
        VkPhysicalDevice physicalDevice,
        VkDevice device,
        VulkanSharedDeviceState* sharedDeviceState
    )
    {
        assert(instance != VK_NULL_HANDLE && "Vulkan instance was VK_NULL_HANDLE!");
        assert(physicalDevice != VK_NULL_HANDLE && "Vulkan physical device was VK_NULL_HANDLE!");
        assert(device != VK_NULL_HANDLE && "Vulkan device was VK_NULL_HANDLE!");
        assert(sharedDeviceState != nullptr && "Shared device state was null!");

        return new VulkanObjectFactory(instance, physicalDevice, device, sharedDeviceState);
    }
    
    VulkanSwapchain* VulkanObjectFactory::CreateSwapchain(GraphicsSwapchainDesc const& swapchainDesc)
    {
        // Create Vulkan surface
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        if (!SDL_Vulkan_CreateSurface(swapchainDesc.window, _instance, nullptr, &surface)) {
            return nullptr;
        }

        // Create the new swapchain and configure
        VulkanSwapchainDesc vulkanSwapchainDesc{};
        vulkanSwapchainDesc.physicalDevice = _physicalDevice;
        vulkanSwapchainDesc.device = _device;
        vulkanSwapchainDesc.surface = surface;
        vulkanSwapchainDesc.sharedDeviceState = _sharedDeviceState;
        vulkanSwapchainDesc.width = swapchainDesc.width;
        vulkanSwapchainDesc.height = swapchainDesc.height;
        vulkanSwapchainDesc.syncInterval = swapchainDesc.syncInterval;
        vulkanSwapchainDesc.allowTearing = swapchainDesc.allowTearing;

        VulkanSwapchain* swapchain = new VulkanSwapchain{};
        if (!VulkanSwapchainHelpers::ConfigureSwapchain(vulkanSwapchainDesc, *swapchain))
        {
            delete swapchain;
            return nullptr;
        }

        return swapchain;
    }
} // namespace Bonsai
