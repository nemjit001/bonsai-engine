#pragma once
#ifndef BONSAI_VULKAN_RENDER_CONTEXT_HPP
#define BONSAI_VULKAN_RENDER_CONTEXT_HPP

#include <cstdint>
#include <vector>
#include "Gfx/Vulkan/VulkanGraphicsTypes.hpp"

namespace Bonsai
{
    class VulkanRenderContext
    {
    private:
        VulkanRenderContext(
            VkDevice device,
            VulkanSharedDeviceState* sharedDeviceState,
            std::vector<VulkanFrameContext>&& frames
        );

    public:
        virtual ~VulkanRenderContext();

        VulkanRenderContext(VulkanRenderContext const&) = delete;
        VulkanRenderContext& operator=(VulkanRenderContext const&) = delete;

        /// @brief Try to create a Vulkan render context.
        /// @param device Logical device to use in the render context.
        /// @param sharedDeviceState Shared device state.
        /// @return 
        [[nodiscard]] static VulkanRenderContext* TryCreate(
            VkDevice device,
            VulkanSharedDeviceState* sharedDeviceState
        );

        /// @brief Start a Vulkan frame.
        /// @param swapchain Swapchains participating in this frame.
        /// @param swapchainCount Number of swapchains in the swapchains array.
        /// @return Frame context for the frame.
        VulkanFrameContext* StartFrame(GraphicsSwapchain** swapchains, size_t swapchainCount);

        /// @brief Record a Vulkan frame.
        /// @param frameContext Frame context for the frame.
        void RecordFrame(VulkanFrameContext* frameContext);

        /// @brief Submit a Vulkan frame.
        /// @param frameContext Frame context for the frame.
        void SubmitFrame(VulkanFrameContext* frameContext);

        /// @brief Wait for all frames to finish executing.
        void waitForAllFrames();

    private:
        /// @brief Wait for a frame to finish executing. This will release frame resources
        /// that were acquired during previous exeuction.
        /// @param frameContext Frame context to wait on.
        /// @return A boolean indicating success.
        bool waitForFrame(VulkanFrameContext& frameContext);

        /// @brief Prepare a frame for rendering.
        /// @param frameContext Frame context to prepare for rendering.
        /// @return A boolean indicating success.
        bool prepareFrame(VulkanFrameContext& frameContext);

    private:
        VkDevice _device;
        VulkanSharedDeviceState* _sharedDeviceState;
        std::vector<VulkanFrameContext> _frames;
        uint64_t _currentFrameIndex;
    };
} // namespace Bonsai

#endif //BONSAI_VULKAN_RENDER_CONTEXT_HPP
