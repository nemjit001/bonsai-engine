#include "VulkanRenderContext.hpp"

#include <volk.h>

namespace Bonsai
{
    VulkanRenderContext::VulkanRenderContext(
        VkDevice device,
        VkQueue graphicsQueue,
        uint64_t framesInFlight,
        std::vector<VulkanFrameContext> const& frames
    )
        :
        _device(device),
        _graphicsQueue(graphicsQueue),
        _framesInFlight(framesInFlight),
        _frames(frames)
    {
        //
    }

    VulkanRenderContext::~VulkanRenderContext()
    {
        for (auto const& frame : _frames)
        {
            vkDestroyCommandPool(_device, frame.graphicsCommandPool, nullptr);
            vkDestroyFence(_device, frame.frameReadyFence, nullptr);
        }
    }

    VulkanRenderContext* VulkanRenderContext::TryCreate(
        VkDevice device,
        VkQueue graphicsQueue,
        uint64_t framesInFlight
    )
    {
        // Create frames
        std::vector<VulkanFrameContext> frames(framesInFlight);
        for (auto& frame : frames)
        {
            // Create frame ready fence
            VkFenceCreateInfo fenceCreateInfo{};
            fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceCreateInfo.pNext = nullptr;
            fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            VkFence frameReadyFence = VK_NULL_HANDLE;
            if (VK_CHECK_FAILED(vkCreateFence(device, &fenceCreateInfo, nullptr, &frameReadyFence))) {
                return nullptr;
            }

            // Create graphics command pool and frame command buffer
            // TODO(nemjit001): Create frame command state
            VkCommandPool graphicsCommandPool = VK_NULL_HANDLE;
            VkCommandBuffer frameCommandBuffer = VK_NULL_HANDLE;
            
            // Populate frame context
            frame.frameReadyFence = frameReadyFence;
            frame.graphicsCommandPool = graphicsCommandPool;
            frame.frameCommandBuffer = frameCommandBuffer;
        }

        return new VulkanRenderContext(device, graphicsQueue, framesInFlight, frames);
    }

    VulkanFrameContext* VulkanRenderContext::StartFrame()
    {
        // Acquire the frame
        VulkanFrameContext& frame = _frames[_currentFrameIndex % _framesInFlight];
        vkWaitForFences(_device, 1, &frame.frameReadyFence, VK_TRUE, UINT64_MAX);

        // Reset fence and command buffer
        vkResetFences(_device, 1, &frame.frameReadyFence);
        vkResetCommandBuffer(frame.frameCommandBuffer, 0);

        // Begin frame command buffer
        VkCommandBufferBeginInfo frameCommandBufferBeginInfo{};
        frameCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        frameCommandBufferBeginInfo.pNext = nullptr;
        frameCommandBufferBeginInfo.flags = 0;
        frameCommandBufferBeginInfo.pInheritanceInfo = nullptr;

        vkBeginCommandBuffer(frame.frameCommandBuffer, &frameCommandBufferBeginInfo);

        return &frame;
    }

    void VulkanRenderContext::SubmitFrame(VulkanFrameContext* frameContext)
    {
        // Check for a valid frame context
        if (frameContext == nullptr) {
            return;
        }

        // End frame command buffer
        VulkanFrameContext& frame = *frameContext;
        vkEndCommandBuffer(frame.frameCommandBuffer);

        // Submit frame
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext = nullptr;
        submitInfo.waitSemaphoreCount = 0;
        submitInfo.pWaitSemaphores = nullptr;
        submitInfo.pWaitDstStageMask = nullptr;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &frame.frameCommandBuffer;
        submitInfo.signalSemaphoreCount = 0;
        submitInfo.pSignalSemaphores = nullptr;

        vkQueueSubmit(_graphicsQueue, 1, &submitInfo, frame.frameReadyFence);

        // Bump frame index to next frame
        _currentFrameIndex++;
    }
} // namespace Bonsai
