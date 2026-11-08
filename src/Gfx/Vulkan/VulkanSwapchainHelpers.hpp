#pragma once
#ifndef BONSAI_VULKAN_SWAPCHAIN_HELPERS
#define BONSAI_VULKAN_SWAPCHAIN_HELPERS

#include "Gfx/Vulkan/VulkanGraphicsTypes.hpp"

namespace Bonsai
{
    struct VulkanSwapchainDesc
    {
        VkPhysicalDevice physicalDevice;
        VkDevice device;
        VkSurfaceKHR surface;
        VulkanSharedDeviceState* sharedDeviceState;
        uint32_t width;
        uint32_t height;
        uint32_t syncInterval;
        bool allowTearing;
    };

    namespace VulkanSwapchainHelpers
    {
        /// @brief Configure a Vulkan swapchain.
        /// @param swapchainDesc Vulkan swapchain description.
        /// @param swapchain Swapchain to configure.
        /// @return A boolean indicating success.
        [[nodiscard]] bool ConfigureSwapchain(VulkanSwapchainDesc const& swapchainDesc, VulkanSwapchain& swapchain);

        /// @brief Reconfigure a Vulkan swapchain.
        /// @param swapchain Swapchain to reconfigure.
        /// @return A boolean indicating success.
        [[nodiscard]] bool ReconfigureSwapchain(VulkanSwapchain& swapchain);

        /// @brief Resize a Vulkan swapchain.
        /// @param swapchain Swapchain to resize.
        /// @param width New swapchain width.
        /// @param height New swapchain height.
        /// @return A boolean indicating success.
        [[nodiscard]] bool ResizeSwapchain(VulkanSwapchain& swapchain, uint32_t width, uint32_t height);

        /// @brief Destroy swapchain state.
        /// @param swapchain Swapchain to destroy state for.
        void DestroySwapchain(VulkanSwapchain& swapchain);

        /// @brief Acquire the next image for a Vulkan swapchain.
        /// @param swapchain Swapchain to use for image acquisition.
        /// @return A boolean indicating success.
        [[nodiscard]] bool AcquireNextImage(VulkanSwapchain& swapchain);

        /// @brief Register a Vulkan swapchain with a frame's swapchain info.
        /// @param swapchainInfo Swapchain info to register with.
        /// @param swapchain Swapchain to register.
        void RegisterWithFrame(VulkanFrameSwapchainInfo& swapchainInfo, VulkanSwapchain const& swapchain);

        /// @brief Prepare a swapchain for rendering.
        /// @param swapchain Swapchain to prep.
        /// @param commandBuffer Command buffer to record commands into.
        void PrepareForRendering(VulkanSwapchain& swapchain, VkCommandBuffer commandBuffer);

        /// @brief Prepare a swapchain for presentation.
        /// @param swapchain Swapchain to prep.
        /// @param commandBuffer Command buffer to record commands into.
        void PrepareForPresent(VulkanSwapchain& swapchain, VkCommandBuffer commandBuffer);
    } // namespace VulkanSwapchainHelpers
} // namespace Bonsai

#endif //BONSAI_VULKAN_SWAPCHAIN_HELPERS
