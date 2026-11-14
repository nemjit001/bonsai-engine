#pragma once
#ifndef BONSAI_VULKAN_COMMAND_RECORDER_HPP
#define BONSAI_VULKAN_COMMAND_RECORDER_HPP

#define VK_NO_PROTOTYPES

#include <vulkan/vulkan.h>

namespace Bonsai
{
    struct GraphicsCommand;
    class GraphicsCommandBufferImpl;

    /// @brief The VulkanGrapCommandRecorder records graphics command into a Vulkan command buffer.
    class VulkanCommandRecorder
    {
    private:
        VulkanCommandRecorder() = default;

    public:
        /// @brief Record a Vulkan command buffer using a set of recorded graphics commands.
        /// @param commandBuffer Vulkan command buffer to record into.
        /// @param recordedCommands Recorded graphics commands.
        static void RecordCommandBuffer(VkCommandBuffer commandBuffer, GraphicsCommandBufferImpl const* recordedCommands);

    private:
        /// @brief Record a graphics command into a Vulkan command buffer.
        /// @param commandBuffer Vulkan command buffer to record into.
        /// @param command Graphics command to record.
        static void RecordCommand(VkCommandBuffer commandBuffer, GraphicsCommand const& command);
    };
} // namespace Bonsai

#endif //BONSAI_VULKAN_COMMAND_RECORDER_HPP
