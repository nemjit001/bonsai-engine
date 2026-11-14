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
    
    /// @brief Get the Vulkan sample count flag bits based on a given sample count.
    /// @param sampleCount Sample count to translate.
    /// @return The Vulkan sample count flag bits.
    [[nodiscard]] VkSampleCountFlagBits GetVulkanSampleCountFlagBits(size_t const& sampleCount);

    /// @brief Get the Vulkan format for a given graphics format.
    /// @param format Format to translate.
    /// @return The Vulkan format.
    [[nodiscard]] VkFormat GetVulkanFormat(GraphicsFormat const& format);

    /// @brief Get the Vulkan image type for a given graphics texture type.
    /// @param textureType Texture type to translate.
    /// @return The Vulkan image type.
    [[nodiscard]] VkImageType GetVulkanImageType(GraphicsTextureType const& textureType);

    /// @brief Get the Vulkan image type for a given graphics texture tiling.
    /// @param textureTiling Texture tiling to translate.
    /// @return The Vulkan image tiling.
    [[nodiscard]] VkImageTiling GetVulkanImageTiling(GraphicsTextureTiling const& textureTiling);

    /// @brief Get the Vulkan filter for a given graphics filter.
    /// @param filter Filter to translate.
    /// @return The Vulkan filter.
    [[nodiscard]] VkFilter GetVulkanFilter(GraphicsFilter const& filter);

    /// @brief Get the Vulkan sampler mipmap mode for a given graphics sampler mipmap mode.
    /// @param mipmapMode Sampler mipmap mode to translate.
    /// @return The Vulkan sampler mipmap mode.
    [[nodiscard]] VkSamplerMipmapMode GetVulkanSamplerMipmapMode(GraphicsSamplerMipmapMode const& mipmapMode);

    /// @brief Get the Vulkan sampler address mode for a given graphics sampler address mode.
    /// @param addressMode Sampler address mode to translate.
    /// @return The Vulkan sampler address mode.
    [[nodiscard]] VkSamplerAddressMode GetVulkanSamplerAddressMode(GraphicsSamplerAddressMode const& addressMode);

    /// @brief Get the Vulkan border color for a given graphics border color.
    /// @param borderColor Border color to translate.
    /// @return The Vulkan border color.
    [[nodiscard]] VkBorderColor GetVulkanBorderColor(GraphicsBorderColor const& borderColor);

    /// @brief Get the Vulkan compare op for a given graphics compare op.
    /// @param compareOp Compare op to translate.
    /// @return The Vulkan compare op.
    [[nodiscard]] VkCompareOp GetVulkanCompareOp(GraphicsCompareOp const& compareOp);

    /// @brief Get the Vulkan image aspect flags for a given graphics texture aspect mask.
    /// @param aspectMask Aspect mask to translate.
    /// @return The Vulkan image aspect flags.
    [[nodiscard]] VkImageAspectFlags GetVulkanImageAspectFlags(GraphicsTextureAspectFlags const& aspectMask);

    /// @brief Get the Vulkan buffer usage flags for a given set of graphics buffer usage flags.
    /// @param usageFlags Usage flags to translate.
    /// @return The Vulkan buffer usage flags.
    [[nodiscard]] VkBufferUsageFlags GetVulkanBufferUsageFlags(GraphicsBufferUsageFlags const& usageFlags);

    /// @brief Get the Vulkan image usage flags for a given set of graphics texture usage flags.
    /// @param usageFlags Usage flags to translate.
    /// @return The Vulkan image usage flags.
    [[nodiscard]] VkImageUsageFlags GetVulkanImageUsageFlags(GraphicsTextureUsageFlags const& usageFlags);
} // namespace Bonsai::VulkanGraphicsHelpers

#endif //BONSAI_VULKAN_GRAPHICS_HELPERS_HPP
