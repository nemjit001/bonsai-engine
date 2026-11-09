#pragma once
#ifndef BONSAI_VULKAN_GRAPHICS_TYPES_HPP
#define BONSAI_VULKAN_GRAPHICS_TYPES_HPP

#define VK_NO_PROTOTYPES

#include <vector>
#include <vulkan/vulkan.h>
#include "Gfx/GraphicsTypesInternal.hpp"

namespace Bonsai
{
    class VulkanSwapchain;

    /// @brief Vulkan buffer implementation.
    class VulkanBuffer : public GraphicsBuffer
    {
    public:
        VulkanBuffer(VkDevice device, VkDeviceMemory memory, VkBuffer buffer);

    protected:
        ~VulkanBuffer() override;

    private:
        VkDevice _device;
        VkDeviceMemory _memory;
        VkBuffer _buffer;
    };

    /// @brief Shared state for Vulkan devices.
    struct VulkanSharedDeviceState
    {
        uint64_t framesInFlight;
        VkPhysicalDeviceMemoryProperties2 physicalDeviceMemoryProperties2;
        uint32_t graphicsQueueFamily;
        uint32_t presentQueueFamily;
        uint32_t computeQueueFamily;
        uint32_t transferQueueFamily;
        VkQueue graphicsQueue;
        VkQueue presentQueue;
        VkQueue computeQueue;
        VkQueue transferQueue;
    };

    /// @brief Vulkan swapchain info for tracking active swap chains for a frame.
    struct VulkanFrameSwapchainInfo
    {
        std::vector<VkSwapchainKHR> swapchains;
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkPipelineStageFlags> dstWaitStageMasks;
        std::vector<uint32_t> imageIndices;
    };
    
    /// @brief Vulkan frame context implementation. Exposes Vulkan frame state as public member variables.
    class VulkanFrameContext : public FrameContext
    {
    public:
        VkFence frameReadyFence;
        VkSemaphore renderingFinishedSemaphore;
        VkCommandPool graphicsCommandPool;
        VkCommandBuffer frameCommandBuffer;
        std::vector<VulkanSwapchain*> participatingSwapchains;
        VulkanFrameSwapchainInfo swapchainInfo;
    };
} // namespace Bonsai

#endif //BONSAI_VULKAN_GRAPHICS_TYPES_HPP
