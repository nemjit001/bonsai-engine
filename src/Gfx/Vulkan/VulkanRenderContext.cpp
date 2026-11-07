#include "VulkanRenderContext.hpp"

#include <cassert>
#include <volk.h>

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
            vkDestroyFence(_device, frame.frameReadyFence, nullptr);
        }
    }

    VulkanRenderContext* VulkanRenderContext::TryCreate(
        VkDevice device,
        VulkanSharedDeviceState* sharedDeviceState
    )
    {
        assert(device != VK_NULL_HANDLE && "Device was VK_NULL_HANDLE!");
        assert(sharedDeviceState != nullptr && "Shared device state was null!");

        // Create frames
        std::vector<VulkanFrameContext> frames(sharedDeviceState->framesInFlight);
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
            frame.graphicsCommandPool = graphicsCommandPool;
            frame.frameCommandBuffer = frameCommandBuffer;
        }

        return new VulkanRenderContext(device, sharedDeviceState, frames);
    }

    VulkanFrameContext* VulkanRenderContext::StartFrame()
    {
        // Acquire the frame by waiting on the frame fence
        VulkanFrameContext& frame = _frames[_currentFrameIndex % _sharedDeviceState->framesInFlight];
        if (VK_CHECK_FAILED(vkWaitForFences(_device, 1, &frame.frameReadyFence, VK_TRUE, UINT64_MAX))) {
            return nullptr;
        }

        // Reset fence and command buffer
        if (VK_CHECK_FAILED(vkResetFences(_device, 1, &frame.frameReadyFence))
            || VK_CHECK_FAILED(vkResetCommandBuffer(frame.frameCommandBuffer, 0))) {
                return nullptr;
            }

        // Begin frame command buffer
        VkCommandBufferBeginInfo frameCommandBufferBeginInfo{};
        frameCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        frameCommandBufferBeginInfo.pNext = nullptr;
        frameCommandBufferBeginInfo.flags = 0;
        frameCommandBufferBeginInfo.pInheritanceInfo = nullptr;

        if (VK_CHECK_FAILED(vkBeginCommandBuffer(frame.frameCommandBuffer, &frameCommandBufferBeginInfo))) {
            return nullptr;
        }

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
        if (VK_CHECK_FAILED(vkEndCommandBuffer(frame.frameCommandBuffer))) {
            return;
        }

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

        vkQueueSubmit(_sharedDeviceState->graphicsQueue, 1, &submitInfo, frame.frameReadyFence);

        // Bump frame index to next frame
        _currentFrameIndex++;
    }
} // namespace Bonsai
