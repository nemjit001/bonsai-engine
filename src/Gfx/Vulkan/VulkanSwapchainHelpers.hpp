#pragma once
#ifndef BONSAI_VULKAN_SWAPCHAIN_HELPERS
#define BONSAI_VULKAN_SWAPCHAIN_HELPERS

#include "Gfx/Vulkan/VulkanGraphicsTypes.hpp"

namespace Bonsai::VulkanSwapchainHelpers
{
    /// @brief Acquire the next image for a swapchain.
    /// @param swapchain Swapchain to use for image acquisition.
    /// @return A boolean indicating success.
    [[nodiscard]] bool AcquireNextImage(VulkanSwapchain& swapchain);

    /// @brief Register a swapchain with a frame's swapchain info.
    /// @param swapchainInfo Swapchain info to register with.
    /// @param swapchain Swapchain to register.
    void RegisterWithFrame(VulkanFrameSwapchainInfo& swapchainInfo, VulkanSwapchain const& swapchain);
} // namespace Bonsai::VulkanSwapchainHelpers

#endif //BONSAI_VULKAN_SWAPCHAIN_HELPERS
