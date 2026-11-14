#pragma once
#ifndef BONSAI_VULKAN_TRANSFER_CONTEXT_HPP
#define BONSAI_VULKAN_TRANSFER_CONTEXT_HPP

#include <cstdint>
#include <vector>
#include <Gfx/Vulkan/VulkanGraphicsTypes.hpp>

namespace Bonsai
{
    /// @brief The Vulkan transfer context manages Vulkan transfer operations.
    class VulkanTransferContext
    {
    public:
        /// @brief Number of transfer batches ready for use in the transfer context.
        static constexpr uint32_t TransferBatchesInUse = 16u;

    private:
        VulkanTransferContext(
            VkDevice device,
            VulkanSharedDeviceState* sharedDeviceState,
            std::vector<VulkanTransferBatch>&& transferBatches
        );

    public:
        virtual ~VulkanTransferContext();

        VulkanTransferContext(VulkanTransferContext const&) = delete;
        VulkanTransferContext& operator=(VulkanTransferContext const&) = delete;
        
        /// @brief Try to create a Vulkan transfer context.
        /// @param device Logical device to use in the render context.
        /// @param sharedDeviceState Shared device state.
        /// @return 
        [[nodiscard]] static VulkanTransferContext* TryCreate(
            VkDevice device,
            VulkanSharedDeviceState* sharedDeviceState
        );

        /// @brief Start a Vulkan transfer batch.
        /// @return Transfer batch for pending transfer.
        VulkanTransferBatch* StartTransferBatch();

        /// @brief Record a Vulkan transfer batch.
        /// @param transferBatch Transfer batch to record.
        void RecordTransferBatch(VulkanTransferBatch* transferBatch);

        /// @brief Submit a Vulkan transfer batch.
        /// @param transferBatch Transfer batch to submit.
        void SubmitTransferBatch(VulkanTransferBatch* transferBatch);

        /// @brief Wait for a Vulkan transfer batch to complete.
        /// @param transferBatch Transfer batch to wait for.
        void WaitForTransferBatch(VulkanTransferBatch* transferBatch);

    private:
        VkDevice _device;
        VulkanSharedDeviceState* _sharedDeviceState;
        std::vector<VulkanTransferBatch> _transferBatches;
        std::vector<uint32_t> _transferBatchFreeList; //< Free list for tracking batch indices
    };
} // namespace Bonsai

#endif //BONSAI_VULKAN_TRANSFER_CONTEXT_HPP
