#include "VulkanTransferContext.hpp"

#include <cassert>
#include <volk.h>
#include "Core/Logger.hpp"
#include "Gfx/Vulkan/VulkanCommandRecorder.hpp"
#include "Gfx/Vulkan/VulkanGraphicsHelpers.hpp"

namespace Bonsai
{
    VulkanTransferContext::VulkanTransferContext(
        VkDevice device,
        VulkanSharedDeviceState* sharedDeviceState,
        std::vector<VulkanTransferBatch>&& transferBatches
    )
        :
        _device(device),
        _sharedDeviceState(sharedDeviceState),
        _transferBatches(std::move(transferBatches)),
        _transferBatchFreeList(TransferBatchesInUse)
    {
        // Populate free list :)
        for (size_t i = 0; i < _transferBatchFreeList.size(); i++) {
            _transferBatchFreeList[i] = i;
        }
    }

    VulkanTransferContext::~VulkanTransferContext()
    {
        for (auto& batch : _transferBatches)
        {
            vkDestroyCommandPool(_device, batch.transferCommandPool, nullptr);
            vkDestroyFence(_device, batch.transferFinishedFence, nullptr);
        }
    }

    VulkanTransferContext* VulkanTransferContext::TryCreate(
        VkDevice device,
        VulkanSharedDeviceState* sharedDeviceState
    )
    {
        assert(device != VK_NULL_HANDLE && "Vulkan device was VK_NULL_HANDLE!");
        assert(sharedDeviceState != nullptr && "Shared device state was null!");

        // Create transfer batches
        std::vector<VulkanTransferBatch> transferBatches(TransferBatchesInUse);
        uint32_t batchIndex = 0;
        for (auto& batch : transferBatches)
        {
            // Create transfer fence
            VkFenceCreateInfo transferFinishedFenceCreateInfo{};
            transferFinishedFenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            transferFinishedFenceCreateInfo.pNext = nullptr;
            transferFinishedFenceCreateInfo.flags = 0;

            VkFence transferFinishedFence = VK_NULL_HANDLE;
            if (VK_CHECK_FAILED(vkCreateFence(device, &transferFinishedFenceCreateInfo, nullptr, &transferFinishedFence))) {
                return nullptr;
            }

            // Create transfer command pool
            VkCommandPoolCreateInfo transferCommandPoolCreateInfo{};
            transferCommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            transferCommandPoolCreateInfo.pNext = nullptr;
            transferCommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            transferCommandPoolCreateInfo.queueFamilyIndex = sharedDeviceState->transferQueueFamily;

            VkCommandPool transferCommandPool = VK_NULL_HANDLE;
            if (VK_CHECK_FAILED(vkCreateCommandPool(device, &transferCommandPoolCreateInfo, nullptr, &transferCommandPool))) {
                return nullptr;
            }

            // Allocate command buffer
            VkCommandBufferAllocateInfo transferCommandBufferAllocateInfo{};
            transferCommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            transferCommandBufferAllocateInfo.pNext = nullptr;
            transferCommandBufferAllocateInfo.commandPool = transferCommandPool;
            transferCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            transferCommandBufferAllocateInfo.commandBufferCount = 1;

            VkCommandBuffer transferCommandBuffer = VK_NULL_HANDLE;
            if (VK_CHECK_FAILED(vkAllocateCommandBuffers(device, &transferCommandBufferAllocateInfo, &transferCommandBuffer))) {
                return nullptr;
            }

            // Populate transfer batch
            batch.batchIndex = batchIndex;
            batch.transferFinishedFence = transferFinishedFence;
            batch.transferCommandPool = transferCommandPool;
            batch.transferCommandBuffer = transferCommandBuffer;

            // Bump batch index
            batchIndex++;
        }

        return new VulkanTransferContext(device, sharedDeviceState, std::move(transferBatches));
    }

    VulkanTransferBatch* VulkanTransferContext::StartTransferBatch()
    {
        // Check for a ready transfer batch
        if (_transferBatchFreeList.empty()) {
            return nullptr;
        }

        // Get next batch index
        uint32_t const batchIndex = _transferBatchFreeList.back();
        _transferBatchFreeList.pop_back();

        // Get next transfer batch
        VulkanTransferBatch& transferBatch = _transferBatches[batchIndex];
        assert(transferBatch.batchIndex == batchIndex && "Mismatch between transferBatch->batchIndex and selected batchIndex!");

        return &transferBatch;
    }

    void VulkanTransferContext::RecordTransferBatch(VulkanTransferBatch* transferBatch)
    {
        // Check for valid transfer batch
        if (transferBatch == nullptr) {
            return;
        }

        // Begin command buffer recording
        VkCommandBufferBeginInfo commandBufferBeginInfo{};
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        commandBufferBeginInfo.pNext = nullptr;
        commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        commandBufferBeginInfo.pInheritanceInfo = nullptr;

        if (VK_CHECK_FAILED(vkBeginCommandBuffer(transferBatch->transferCommandBuffer, &commandBufferBeginInfo))) {
            return;
        }

        // Record transfer commands
        for (auto const& commandBuffer : transferBatch->commandBuffers)
        {
            VulkanCommandRecorder::RecordCommandBuffer(transferBatch->transferCommandBuffer, commandBuffer);
            commandBuffer->Reset();
        }

        // End command buffer recording
        if (VK_CHECK_FAILED(vkEndCommandBuffer(transferBatch->transferCommandBuffer))) {
            return;
        }
    }

    void VulkanTransferContext::SubmitTransferBatch(VulkanTransferBatch* transferBatch)
    {
        // Check for valid transfer batch
        if (transferBatch == nullptr) {
            return;
        }

        // Submit transfer batch
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext = nullptr;
        submitInfo.waitSemaphoreCount = 0;
        submitInfo.pWaitSemaphores = nullptr;
        submitInfo.pWaitDstStageMask = nullptr;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &transferBatch->transferCommandBuffer;
        submitInfo.signalSemaphoreCount = 0;
        submitInfo.pSignalSemaphores = nullptr;

        if (VK_CHECK_FAILED(vkQueueSubmit(_sharedDeviceState->transferQueue, 1, &submitInfo, transferBatch->transferFinishedFence))) {
            return;
        }        
    }

    void VulkanTransferContext::WaitForTransferBatch(VulkanTransferBatch* transferBatch)
    {
        // Check for valid transfer batch
        if (transferBatch == nullptr) {
            return;
        }

        // Wait for transfer to complete
        if (VK_CHECK_FAILED(vkWaitForFences(_device, 1, &transferBatch->transferFinishedFence, VK_TRUE, UINT64_MAX))) {
            return;
        }

        // Mark transfer batch as free
        _transferBatchFreeList.push_back(transferBatch->batchIndex);

        // Clear recorded command buffers
        transferBatch->commandBuffers.clear();

        // Reset fence and command buffer
        if (VK_CHECK_FAILED(vkResetFences(_device, 1, &transferBatch->transferFinishedFence))
            || VK_CHECK_FAILED(vkResetCommandBuffer(transferBatch->transferCommandBuffer, 0))
        ) {
            SPDLOG_WARN("Failed to reset transfer batch fence and command buffer!");
        }
    }
} // namespace Bonsai
