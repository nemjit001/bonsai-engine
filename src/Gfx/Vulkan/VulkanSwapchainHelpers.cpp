#include "VulkanSwapchainHelpers.hpp"

#include <volk.h>
#include "Core/Logger.hpp"

namespace Bonsai::VulkanSwapchainHelpers
{
    bool AcquireNextImage(VulkanSwapchain& swapchain)
    {
        // Check if swapchain needs to be recreated
        if (swapchain.recreateSwapchain)
        {
            // Try to recreate swapchain once
            swapchain.recreateSwapchain = false;
            // TODO(nemjit001): Recreate swapchain here
            return false;
        }

        // Get image semaphore
        VkSemaphore const imageAvailableSemaphore = swapchain.imageAvailableSemaphores[swapchain.currentImageSemaphoreIndex];

        // Acquire next swapchain image
        uint32_t imageIndex = 0;
        VkResult const acquireResult = vkAcquireNextImageKHR(swapchain.device, swapchain.swapchain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
        if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR || acquireResult == VK_SUBOPTIMAL_KHR)
        {
            swapchain.recreateSwapchain = true;
            return false;
        }
        else if (VK_CHECK_FAILED(acquireResult))
        {
            SPDLOG_ERROR("Fatal error during swapchain image acquisition");
            return false;
        }

        // Update swapchain state
        swapchain.currentImageSemaphoreIndex = (swapchain.currentImageSemaphoreIndex + 1) % swapchain.imageAvailableSemaphores.size();
        swapchain.activeImageIndex = imageIndex;
        swapchain.recreateSwapchain = false;
        return true;
    }

    void RegisterWithFrame(VulkanFrameSwapchainInfo& swapchainInfo, VulkanSwapchain const& swapchain)
    {
        swapchainInfo.swapchains.push_back(swapchain.swapchain);
        swapchainInfo.imageAvailableSemaphores.push_back(swapchain.imageAvailableSemaphores[swapchain.currentImageSemaphoreIndex]);
        swapchainInfo.imageIndices.push_back(swapchain.activeImageIndex);
        swapchainInfo.dstWaitStageMasks.push_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    }
} // namespace Bonsai::VulkanSwapchainHelpers
