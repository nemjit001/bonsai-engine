#pragma once
#ifndef BONSAI_VULKAN_COMMAND_RECORDER_HPP
#define BONSAI_VULKAN_COMMAND_RECORDER_HPP

#define VK_NO_PROTOTYPES

#include "Gfx/GraphicsTypes.hpp"
#include <vulkan/vulkan.h>

namespace Bonsai
{
    struct GraphicsCommand;
    class GraphicsCommandBuffer;

    /// @brief Combination of pipeline stage mask and access flags that make up a resource state.
    struct VulkanResourceAccessState
    {
        VkPipelineStageFlags2 stageMask;
        VkAccessFlags2 accessMask;
    };

    /// @brief The VulkanGrapCommandRecorder records graphics command into a Vulkan command buffer.
    class VulkanCommandRecorder
    {
    private:
        VulkanCommandRecorder() = default;

    public:
        /// @brief Record a Vulkan command buffer using a set of recorded graphics commands.
        /// @param commandBuffer Vulkan command buffer to record into.
        /// @param recordedCommands Recorded graphics commands.
        static void RecordCommandBuffer(VkCommandBuffer commandBuffer, GraphicsCommandBuffer const* recordedCommands);

    private:
        /// @brief Record a graphics command into a Vulkan command buffer.
        /// @param commandBuffer Vulkan command buffer to record into.
        /// @param command Graphics command to record.
        static void RecordCommand(VkCommandBuffer commandBuffer, GraphicsCommand const& command);

        /// @brief Get the Vulkan resource state based on a graphics resource state value.
        /// @param resourceState Resource state to use.
        /// @return The Vulkan resource access state.
        static VulkanResourceAccessState GetVulkanResourceAccessState(GraphicsResourceState const& resourceState);
    };
} // namespace Bonsai

#endif //BONSAI_VULKAN_COMMAND_RECORDER_HPP
