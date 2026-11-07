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
            std::vector<VulkanFrameContext> const& frames
        );

    public:
        virtual ~VulkanRenderContext();

        VulkanRenderContext(VulkanRenderContext const&) = delete;
        VulkanRenderContext& operator=(VulkanRenderContext const&) = delete;

        /// @brief Try to create a Vulkan render context.
        /// @param device Logical device to use in the render context.
        /// @param sharedDeviceState Shared device state.
        /// @return 
        static VulkanRenderContext* TryCreate(
            VkDevice device,
            VulkanSharedDeviceState* sharedDeviceState
        );

        /// @brief Start a Vulkan frame.
        /// @return Frame context for the frame.
        VulkanFrameContext* StartFrame();

        /// @brief Submit a Vulkan frame.
        /// @param frameContext Frame context for the frame.
        void SubmitFrame(VulkanFrameContext* frameContext);

    private:
        VkDevice _device;
        VulkanSharedDeviceState* _sharedDeviceState;
        std::vector<VulkanFrameContext> _frames;
        uint64_t _currentFrameIndex;
    };
} // namespace Bonsai

#endif //BONSAI_VULKAN_RENDER_CONTEXT_HPP
