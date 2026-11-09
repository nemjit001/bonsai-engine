#pragma once
#ifndef BONSAI_VULKAN_GRAPHICS_HELPERS_HPP
#define BONSAI_VULKAN_GRAPHICS_HELPERS_HPP

#define VK_NO_PROTOTYPES

#include <vector>
#include <vulkan/vulkan.h>
#include "Gfx/GraphicsTypes.hpp"

#define VK_CHECK_SUCCEEDED(result)  (result == VK_SUCCESS)
#define VK_CHECK_FAILED(result)     (result != VK_SUCCESS)

namespace Bonsai::VulkanGraphicsHelpers
{
    /// @brief Find the memory type index for an allocation based on required memory type bits and memory properties.
    /// @param memoryProperties Physical device memory properties.
    /// @param memoryTypeBits Memory type bits that are supported by the allocation.
    /// @param requiredPropertyFlags Memory property flags that are required by the user.
    /// @return The memory type index, or UINT32_MAX on failure.
    [[nodiscard]] uint32_t FindMemoryTypeIndex(
        VkPhysicalDeviceMemoryProperties const& memoryProperties,
        uint32_t const& memoryTypeBits,
        VkMemoryPropertyFlags const& requiredPropertyFlags
    );

    /// @brief Get the Vulkan memory property flags for a given graphics heap type.
    /// @param heapType Heap type to translate.
    /// @return The Vulkan memory property flags.
    [[nodiscard]] VkMemoryPropertyFlags GetVulkanMemoryPropertyFlags(GraphicsHeapType const& heapType);

    /// @brief Get the Vulkan buffer usage flags for a given set of graphics buffer usage flags.
    /// @param usageFlags Usage flags to translate.
    /// @return The Vulkan buffer usage flags.
    [[nodiscard]] VkBufferUsageFlags GetVulkanBufferUsageFlags(GraphicsBufferUsageFlags const& usageFlags);
} // namespace Bonsai::VulkanGraphicsHelpers

#endif //BONSAI_VULKAN_GRAPHICS_HELPERS_HPP
