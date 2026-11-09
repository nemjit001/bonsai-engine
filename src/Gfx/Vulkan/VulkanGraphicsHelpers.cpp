#include "VulkanGraphicsHelpers.hpp"

namespace Bonsai::VulkanGraphicsHelpers
{
    uint32_t FindMemoryTypeIndex(
        VkPhysicalDeviceMemoryProperties const& memoryProperties,
        uint32_t const& memoryTypeBits,
        VkMemoryPropertyFlags const& requiredPropertyFlags
    )
    {
        for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
        {
            VkMemoryType const& memoryType = memoryProperties.memoryTypes[i];
            uint32_t const setMemoryTypeBit = (1 << i); // Set bit is used to check supported memory type bits
            if (IsBitSet(memoryTypeBits, setMemoryTypeBit)
                && (memoryType.propertyFlags & requiredPropertyFlags) == requiredPropertyFlags
            ) {
                return i;
            }
        }

        return UINT32_MAX;
    }

    VkMemoryPropertyFlags GetVulkanMemoryPropertyFlags(GraphicsHeapType const& heapType)
    {
        switch (heapType)
        {
        case GraphicsHeapType_Default:
            return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        case GraphicsHeapType_Upload:
            return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
        case GraphicsHeapType_ReadBack:
            return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        default:
            break;
        }

        // Unknown, fall back to none
        return 0;
    }

    VkBufferUsageFlags GetVulkanBufferUsageFlags(GraphicsBufferUsageFlags const& usageFlags)
    {
        VkBufferUsageFlags output = 0;
        if (IsBitSet(usageFlags, GraphicsBufferUsage_TransferSrc)) {
            output |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        }
        else if (IsBitSet(usageFlags, GraphicsBufferUsage_TransferDst)) {
            output |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        }
        else if (IsBitSet(usageFlags, GraphicsBufferUsage_UniformBuffer)) {
            output |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        }
        else if (IsBitSet(usageFlags, GraphicsBufferUsage_StorageBuffer)) {
            output |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        }
        else if (IsBitSet(usageFlags, GraphicsBufferUsage_IndexBuffer)) {
            output |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        }
        else if (IsBitSet(usageFlags, GraphicsBufferUsage_VertexBuffer)) {
            output |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        }
        else if (IsBitSet(usageFlags, GraphicsBufferUsage_IndirectBuffer)) {
            output |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        }

        return output;
    }
} // namespace Bonsai::VulkanGraphicsHelpers
