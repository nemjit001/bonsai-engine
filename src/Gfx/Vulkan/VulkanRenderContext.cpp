#include "VulkanRenderContext.hpp"

#include <cassert>
#include <volk.h>
#include "Gfx/Vulkan/VulkanSwapchainHelpers.hpp"

namespace Bonsai
{
    VulkanRenderContext::VulkanRenderContext(
        VkDevice device,
        VulkanSharedDeviceState* sharedDeviceState,
        std::vector<VulkanFrameContext> const& frames
    )
        :
        _device(device),
        _sharedDeviceState(sharedDeviceState),
        _frames(frames),
        _currentFrameIndex(0)
    {
        //
    }

    VulkanRenderContext::~VulkanRenderContext()
    {
        for (auto const& frame : _frames)
        {
            vkDestroyCommandPool(_device, frame.graphicsCommandPool, nullptr);
            vkDestroySemaphore(_device, frame.renderingFinishedSemaphore, nullptr);
            vkDestroyFence(_device, frame.frameReadyFence, nullptr);
        }
    }

    VulkanRenderContext* VulkanRenderContext::TryCreate(
        VkDevice device,
        VulkanSharedDeviceState* sharedDeviceState
    )
    {
        assert(device != VK_NULL_HANDLE && "Vulkan device was VK_NULL_HANDLE!");
        assert(sharedDeviceState != nullptr && "Shared device state was null!");

        // Create frames
        std::vector<VulkanFrameContext> frames(sharedDeviceState->framesInFlight);
        for (auto& frame : frames)
        {
            // Create fence
            VkFenceCreateInfo frameReadyFenceCreateInfo{};
            frameReadyFenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            frameReadyFenceCreateInfo.pNext = nullptr;
            frameReadyFenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            VkFence frameReadyFence = VK_NULL_HANDLE;
            if (VK_CHECK_FAILED(vkCreateFence(device, &frameReadyFenceCreateInfo, nullptr, &frameReadyFence))) {
                return nullptr;
            }

            // Create semaphore
            VkSemaphoreCreateInfo renderingFinishedSemaphoreCreateInfo{};
            renderingFinishedSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            renderingFinishedSemaphoreCreateInfo.pNext = nullptr;
            renderingFinishedSemaphoreCreateInfo.flags = 0;

            VkSemaphore renderingFinishedSemaphore = VK_NULL_HANDLE;
            if (VK_CHECK_FAILED(vkCreateSemaphore(device, &renderingFinishedSemaphoreCreateInfo, nullptr, &renderingFinishedSemaphore))) {
                return nullptr;
            }

            // Create graphics command pool and frame command buffer
            VkCommandPoolCreateInfo graphicsCommandPoolCreateInfo{};
            graphicsCommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            graphicsCommandPoolCreateInfo.pNext = nullptr;
            graphicsCommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            graphicsCommandPoolCreateInfo.queueFamilyIndex = sharedDeviceState->graphicsQueueFamily;

            VkCommandPool graphicsCommandPool = VK_NULL_HANDLE;
            if (VK_CHECK_FAILED(vkCreateCommandPool(device, &graphicsCommandPoolCreateInfo, nullptr, &graphicsCommandPool))) {
                return nullptr;
            }

            VkCommandBufferAllocateInfo frameCommandBufferAllocateInfo{};
            frameCommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            frameCommandBufferAllocateInfo.pNext = nullptr;
            frameCommandBufferAllocateInfo.commandPool = graphicsCommandPool;
            frameCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            frameCommandBufferAllocateInfo.commandBufferCount = 1;

            VkCommandBuffer frameCommandBuffer = VK_NULL_HANDLE;
            if (VK_CHECK_FAILED(vkAllocateCommandBuffers(device, &frameCommandBufferAllocateInfo, &frameCommandBuffer))) {
                return nullptr;
            }
            
            // Populate frame context
            frame = VulkanFrameContext{};
            frame.frameReadyFence = frameReadyFence;
            frame.renderingFinishedSemaphore = renderingFinishedSemaphore;
            frame.graphicsCommandPool = graphicsCommandPool;
            frame.frameCommandBuffer = frameCommandBuffer;
        }

        return new VulkanRenderContext(device, sharedDeviceState, frames);
    }

    VulkanFrameContext* VulkanRenderContext::StartFrame(GraphicsSwapchain** swapchains, size_t swapchainCount)
    {
        // Acquire the frame by waiting on the frame fence
        VulkanFrameContext& frame = _frames[_currentFrameIndex % _sharedDeviceState->framesInFlight];
        waitForFrame(frame);
        
        // Bump frame index to next frame
        _currentFrameIndex++;

        // Reset fence and command buffer
        if (VK_CHECK_FAILED(vkResetFences(_device, 1, &frame.frameReadyFence))
            || VK_CHECK_FAILED(vkResetCommandBuffer(frame.frameCommandBuffer, 0))
        ) {
            return nullptr;
        }

        // Clear old swapchain data
        frame.participatingSwapchains.clear();
        frame.swapchainInfo.swapchains.clear();
        frame.swapchainInfo.imageAvailableSemaphores.clear();
        frame.swapchainInfo.dstWaitStageMasks.clear();
        frame.swapchainInfo.imageIndices.clear();

        // Acquire all participating swapchain images
        for (size_t i = 0; i < swapchainCount; i++)
        {
            auto vulkanSwapchain = static_cast<VulkanSwapchain*>(swapchains[i]);
            if (vulkanSwapchain == nullptr) {
                continue;
            }

            // Try to acquire the next swapchain image
            if (!VulkanSwapchainHelpers::AcquireNextImage(*vulkanSwapchain)) {
                continue;
            }

            // Add swapchain to frame swapchain info
            VulkanSwapchainHelpers::RegisterWithFrame(frame.swapchainInfo, *vulkanSwapchain);
            frame.participatingSwapchains.push_back(vulkanSwapchain);
        }

        return &frame;
    }

    void VulkanRenderContext::RecordFrame(VulkanFrameContext* frameContext)
    {
        // Check for a valid frame context
        if (frameContext == nullptr) {
            return;
        }

        // Begin command buffer recording
        VkCommandBufferBeginInfo commandBufferBeginInfo{};
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        commandBufferBeginInfo.pNext = nullptr;
        commandBufferBeginInfo.flags = 0;
        commandBufferBeginInfo.pInheritanceInfo = nullptr;

        if (VK_CHECK_FAILED(vkBeginCommandBuffer(frameContext->frameCommandBuffer, &commandBufferBeginInfo))) {
            return;
        }
        
        // Prepare all swapchains for rendering
        for (auto const& swapchain : frameContext->participatingSwapchains) {
            VulkanSwapchainHelpers::PrepareForRendering(*swapchain, frameContext->frameCommandBuffer);
        }

        // Prepare all swapchains for presentation
        for (auto const& swapchain : frameContext->participatingSwapchains) {
            VulkanSwapchainHelpers::PrepareForPresent(*swapchain, frameContext->frameCommandBuffer);
        }

        // End command buffer recording
        if (VK_CHECK_FAILED(vkEndCommandBuffer(frameContext->frameCommandBuffer))) {
            return;
        }
    }

    void VulkanRenderContext::SubmitFrame(VulkanFrameContext* frameContext)
    {
        // Check for a valid frame context
        if (frameContext == nullptr) {
            return;
        }

        // Get reference to frame
        VulkanFrameContext const& frame = *frameContext;
        bool const hasParticipatingSwapchains = !frame.participatingSwapchains.empty();

        // Submit frame
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext = nullptr;
        submitInfo.waitSemaphoreCount = static_cast<uint32_t>(frame.swapchainInfo.imageAvailableSemaphores.size());
        submitInfo.pWaitSemaphores = frame.swapchainInfo.imageAvailableSemaphores.data();
        submitInfo.pWaitDstStageMask = frame.swapchainInfo.dstWaitStageMasks.data();
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &frame.frameCommandBuffer;
        submitInfo.signalSemaphoreCount = 0;
        submitInfo.pSignalSemaphores = nullptr;

        if (hasParticipatingSwapchains)
        {
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = &frame.renderingFinishedSemaphore;
        }

        if (VK_CHECK_FAILED(vkQueueSubmit(_sharedDeviceState->graphicsQueue, 1, &submitInfo, frame.frameReadyFence))) {
            return;
        }

        // Present any participating swapchains in a single present call
        if (hasParticipatingSwapchains)
        {
            VkPresentInfoKHR presentInfo{};
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.pNext = nullptr;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = &frame.renderingFinishedSemaphore;
            presentInfo.swapchainCount = static_cast<uint32_t>(frame.swapchainInfo.swapchains.size());
            presentInfo.pSwapchains = frame.swapchainInfo.swapchains.data();
            presentInfo.pImageIndices = frame.swapchainInfo.imageIndices.data();
            presentInfo.pResults = nullptr;

            if (VK_CHECK_FAILED(vkQueuePresentKHR(_sharedDeviceState->presentQueue, &presentInfo))) {
                return;
            }
        }
    }

    void VulkanRenderContext::waitForAllFrames()
    {
        for (auto const& frame : _frames) {
            waitForFrame(frame);
        }
    }

    void VulkanRenderContext::waitForFrame(VulkanFrameContext const& frameContext)
    {
        if (VK_CHECK_FAILED(vkWaitForFences(_device, 1, &frameContext.frameReadyFence, VK_TRUE, UINT64_MAX))) {
            return;
        }
    }
} // namespace Bonsai
