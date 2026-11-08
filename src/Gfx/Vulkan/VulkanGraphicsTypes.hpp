#pragma once
#ifndef BONSAI_VULKAN_GRAPHICS_TYPES_HPP
#define BONSAI_VULKAN_GRAPHICS_TYPES_HPP

#define VK_NO_PROTOTYPES

#include <vector>
#include <vulkan/vulkan.h>
#include "Gfx/GraphicsTypesInternal.hpp"

#define VK_CHECK_SUCCEEDED(result)  (result == VK_SUCCESS)
#define VK_CHECK_FAILED(result)     (result != VK_SUCCESS)

namespace Bonsai
{
    /// @brief Shared state for Vulkan devices.
    struct VulkanSharedDeviceState
    {
        uint64_t framesInFlight;
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

    /// @brief Vulkan swapchain implementation.
    struct VulkanSwapchain : public GraphicsSwapchain
    {
        VkPhysicalDevice physicalDevice;
        VkDevice device;
        VkSurfaceKHR surface;
        VulkanSharedDeviceState* sharedDeviceState;
        
        uint32_t width;
        uint32_t height;
        uint32_t syncInterval;
        bool allowTearing;

        VkSwapchainKHR swapchain;
        std::vector<VkImage> swapchainImages;
        std::vector<VkSemaphore> imageAvailableSemaphores;
        uint32_t currentImageSemaphoreIndex;
        uint32_t activeImageIndex;
        bool recreateSwapchain;
    };
    
    /// @brief Vulkan frame context implementation.
    struct VulkanFrameContext : public FrameContext
    {
        VkFence frameReadyFence;
        VkSemaphore renderingFinishedSemaphore;
        VkCommandPool graphicsCommandPool;
        VkCommandBuffer frameCommandBuffer;
        std::vector<VulkanSwapchain*> participatingSwapchains;
        VulkanFrameSwapchainInfo swapchainInfo;
    };
} // namespace Bonsai

#endif //BONSAI_VULKAN_GRAPHICS_TYPES_HPP
