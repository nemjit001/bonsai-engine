#include "VulkanObjectFactory.hpp"

#include <cassert>
#include <volk.h>
#include "Gfx/Vulkan/VulkanSwapchain.hpp"

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
        // Create swapchain
        VulkanSwapchain* swapchain = VulkanSwapchain::TryCreate(swapchainDesc.window, _instance, _physicalDevice, _device, _sharedDeviceState);
        if (swapchain == nullptr) {
            return nullptr;
        }
        
        // Configure swapchain
        VulkanSwapchainDesc vulkanSwapchainDesc{};
        vulkanSwapchainDesc.width = swapchainDesc.width;
        vulkanSwapchainDesc.height = swapchainDesc.height;
        vulkanSwapchainDesc.syncInterval = swapchainDesc.syncInterval;
        vulkanSwapchainDesc.allowTearing = swapchainDesc.allowTearing;

        if (!swapchain->Configure(vulkanSwapchainDesc))
        {
            delete swapchain;
            return nullptr;
        }

        return swapchain;
    }
} // namespace Bonsai
