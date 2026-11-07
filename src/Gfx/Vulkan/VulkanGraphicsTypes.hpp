#pragma once
#ifndef BONSAI_VULKAN_GRAPHICS_TYPES_HPP
#define BONSAI_VULKAN_GRAPHICS_TYPES_HPP

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include "Gfx/GraphicsTypesInternal.hpp"

#define VK_CHECK_SUCCEEDED(result)  (result == VK_SUCCESS)
#define VK_CHECK_FAILED(result)     (result != VK_SUCCESS)

namespace Bonsai
{
    /// @brief Frame context used with Vulkan.
    struct VulkanFrameContext : public FrameContext
    {
        VkFence frameReadyFence;
        VkCommandPool graphicsCommandPool;
        VkCommandBuffer frameCommandBuffer;
    };
} // namespace Bonsai

#endif //BONSAI_VULKAN_GRAPHICS_TYPES_HPP
