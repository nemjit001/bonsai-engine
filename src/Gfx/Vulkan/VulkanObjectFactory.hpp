#pragma once
#ifndef BONSAI_VULKAN_OBJECT_FACTORY
#define BONSAI_VULKAN_OBJECT_FACTORY

#define VK_NO_PROTOTYPES

#include <vulkan/vulkan.h>
#include "Gfx/Vulkan/VulkanGraphicsTypes.hpp"

namespace Bonsai
{
    /// @brief Factory for creating Vulkan graphics objects.
    class VulkanObjectFactory
    {
    private:
        VulkanObjectFactory(
            VkInstance instance,
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            VulkanSharedDeviceState* sharedDeviceState
        );

    public:
        virtual ~VulkanObjectFactory();

        VulkanObjectFactory(VulkanObjectFactory const&) = delete;
        VulkanObjectFactory& operator=(VulkanObjectFactory const&) = delete;

        /// @brief Try to create a Vulkan object factory.
        /// @param instance Instance to use in the object factory.
        /// @param physicalDevice Physical device to use in the object factory.
        /// @param device Logical device to use in the object factory.
        /// @param sharedDeviceState Shared device state.
        /// @return 
        [[nodiscard]] static VulkanObjectFactory* TryCreate(
            VkInstance instance,
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            VulkanSharedDeviceState* sharedDeviceState
        );

        /// @brief Create a Vulkan buffer.
        /// @param bufferDesc Buffer description to use for creation.
        /// @return A new Vulkan buffer.
        VulkanBuffer* CreateBuffer(GraphicsBufferDesc const& bufferDesc);

        /// @brief Create a Vulkan swapchain.
        /// @param swapchainDesc Swapchain description to use for creation.
        /// @return A new Vulkan swapchain.
        VulkanSwapchain* CreateSwapchain(GraphicsSwapchainDesc const& swapchainDesc);

    private:
        VkInstance _instance;
        VkPhysicalDevice _physicalDevice;
        VkDevice _device;
        VulkanSharedDeviceState* _sharedDeviceState;
    };
} // namespace Bonsai

#endif //BONSAI_VULKAN_OBJECT_FACTORY
