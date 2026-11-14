#include "VulkanGraphicsHelpers.hpp"

#include "Gfx/GraphicsTypesInternal.hpp"

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

    VkSampleCountFlagBits GetVulkanSampleCountFlagBits(size_t const& sampleCount)
    {
        switch (sampleCount)
        {
        case 0:
        case 1:
            return VK_SAMPLE_COUNT_1_BIT;
        case 2:
            return VK_SAMPLE_COUNT_2_BIT;
        case 4:
            return VK_SAMPLE_COUNT_4_BIT;
        case 8:
            return VK_SAMPLE_COUNT_8_BIT;
        case 16:
            return VK_SAMPLE_COUNT_16_BIT;
        case 32:
            return VK_SAMPLE_COUNT_32_BIT;
        case 64:
            return VK_SAMPLE_COUNT_64_BIT;
        default:
            break;
        }

        return VK_SAMPLE_COUNT_1_BIT;
    }

    VkIndexType GetVulkanIndexType(GraphicsIndexType const& indexType)
    {
        switch (indexType)
        {
        case GraphicsIndexType_UInt16:
            return VK_INDEX_TYPE_UINT16;
        case GraphicsIndexType_UInt32:
            return VK_INDEX_TYPE_UINT32;
        default:
            break;
        }

        return VK_INDEX_TYPE_UINT16;
    }

    VkAttachmentLoadOp GetVulkanAttachmentLoadOp(GraphicsLoadOp const& loadOp)
    {
        switch (loadOp)
        {
        case GraphicsLoadOp_Load:
            return VK_ATTACHMENT_LOAD_OP_LOAD;
        case GraphicsLoadOp_Clear:
            return VK_ATTACHMENT_LOAD_OP_CLEAR;
        case GraphicsLoadOp_DontCare:
            return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        default:
            break;
        }

        return VK_ATTACHMENT_LOAD_OP_LOAD;
    }

    VkAttachmentStoreOp GetVulkanAttachmentStoreOp(GraphicsStoreOp const& storeOp)
    {
        switch (storeOp)
        {
        case GraphicsStoreOp_Store:
            return VK_ATTACHMENT_STORE_OP_STORE;
        case GraphicsStoreOp_DontCare:
            return VK_ATTACHMENT_STORE_OP_DONT_CARE;    
        default:
            break;
        }

        return VK_ATTACHMENT_STORE_OP_STORE;
    }

    VkFormat GetVulkanFormat(GraphicsFormat const& format)
    {
        switch (format)
        {
        case GraphicsFormat_Unknown: return VK_FORMAT_UNDEFINED;
        case GraphicsFormat_R8_UNorm: return VK_FORMAT_R8_UNORM;
        case GraphicsFormat_R8_SNorm: return VK_FORMAT_R8_SNORM;
        case GraphicsFormat_R8_UInt: return VK_FORMAT_R8_UINT;
        case GraphicsFormat_R8_SInt: return VK_FORMAT_R8_SINT;
        case GraphicsFormat_RG8_UNorm: return VK_FORMAT_R8G8_UNORM;
        case GraphicsFormat_RG8_SNorm: return VK_FORMAT_R8G8_SNORM;
        case GraphicsFormat_RG8_UInt: return VK_FORMAT_R8G8_UINT;
        case GraphicsFormat_RG8_SInt: return VK_FORMAT_R8G8_SINT;
        case GraphicsFormat_RGBA8_UNorm: return VK_FORMAT_R8G8B8A8_UNORM;
        case GraphicsFormat_RGBA8_SNorm: return VK_FORMAT_R8G8B8A8_SNORM;
        case GraphicsFormat_RGBA8_UInt: return VK_FORMAT_R8G8B8A8_UINT;
        case GraphicsFormat_RGBA8_SInt: return VK_FORMAT_R8G8B8A8_SINT;
        case GraphicsFormat_RGBA8_UNorm_SRGB: return VK_FORMAT_R8G8B8A8_SRGB;
        case GraphicsFormat_BGRA8_UNorm: return VK_FORMAT_B8G8R8_UNORM;
        case GraphicsFormat_BGRA8_UNorm_SRGB: return VK_FORMAT_B8G8R8_SRGB;
        case GraphicsFormat_R16_UNorm: return VK_FORMAT_R16_UNORM;
        case GraphicsFormat_R16_SNorm: return VK_FORMAT_R16_SNORM;
        case GraphicsFormat_R16_UInt: return VK_FORMAT_R16_UINT;
        case GraphicsFormat_R16_SInt: return VK_FORMAT_R16_SINT;
        case GraphicsFormat_R16_SFloat: return VK_FORMAT_R16_SFLOAT;
        case GraphicsFormat_RG16_UNorm: return VK_FORMAT_R16G16_UNORM;
        case GraphicsFormat_RG16_SNorm: return VK_FORMAT_R16G16_SNORM;
        case GraphicsFormat_RG16_UInt: return VK_FORMAT_R16G16_UINT;
        case GraphicsFormat_RG16_SInt: return VK_FORMAT_R16G16_SINT;
        case GraphicsFormat_RG16_SFloat: return VK_FORMAT_R16G16_SFLOAT;
        case GraphicsFormat_RGBA16_UNorm: return VK_FORMAT_R16G16B16A16_UNORM;
        case GraphicsFormat_RGBA16_SNorm: return VK_FORMAT_R16G16B16A16_SNORM;
        case GraphicsFormat_RGBA16_UInt: return VK_FORMAT_R16G16B16A16_UINT;
        case GraphicsFormat_RGBA16_SInt: return VK_FORMAT_R16G16B16A16_SINT;
        case GraphicsFormat_RGBA16_SFloat: return VK_FORMAT_R16G16B16A16_SFLOAT;
        case GraphicsFormat_R32_UInt: return VK_FORMAT_R32_UINT;
        case GraphicsFormat_R32_SInt: return VK_FORMAT_R32_SINT;
        case GraphicsFormat_R32_SFloat: return VK_FORMAT_R32_SFLOAT;
        case GraphicsFormat_RG32_UInt: return VK_FORMAT_R32G32_UINT;
        case GraphicsFormat_RG32_SInt: return VK_FORMAT_R32G32_SINT;
        case GraphicsFormat_RG32_SFloat: return VK_FORMAT_R32G32_SFLOAT;
        case GraphicsFormat_RGB32_UInt: return VK_FORMAT_R32G32B32_UINT;
        case GraphicsFormat_RGB32_SInt: return VK_FORMAT_R32G32B32_SINT;
        case GraphicsFormat_RGB32_SFloat: return VK_FORMAT_R32G32B32_SFLOAT;
        case GraphicsFormat_RGBA32_UInt: return VK_FORMAT_R32G32B32A32_UINT;
        case GraphicsFormat_RGBA32_SInt: return VK_FORMAT_R32G32B32A32_SINT;
        case GraphicsFormat_RGBA32_SFloat: return VK_FORMAT_R32G32B32A32_SFLOAT;
        case GraphicsFormat_D16_UNorm: return VK_FORMAT_D16_UNORM;
        case GraphicsFormat_D24_UNorm_S8_UInt: return VK_FORMAT_D24_UNORM_S8_UINT;
        case GraphicsFormat_D32_SFloat: return VK_FORMAT_D32_SFLOAT;
        }

        return VK_FORMAT_UNDEFINED;
    }

    VkImageType GetVulkanImageType(GraphicsTextureType const& textureType)
    {
        switch (textureType)
        {
        case GraphicsTextureType_1D:
            return VK_IMAGE_TYPE_1D;
        case GraphicsTextureType_2D:
            return VK_IMAGE_TYPE_2D;
        case GraphicsTextureType_3D:
            return VK_IMAGE_TYPE_3D;
        default:
            break;
        }

        return VK_IMAGE_TYPE_1D;
    }

    VkImageTiling GetVulkanImageTiling(GraphicsTextureTiling const& textureTiling)
    {
        switch (textureTiling)
        {
        case GraphicsTextureTiling_Optimal:
            return VK_IMAGE_TILING_OPTIMAL;
        case GraphicsTextureTiling_Linear:
            return VK_IMAGE_TILING_LINEAR;
        default:
            break;
        }

        return VK_IMAGE_TILING_OPTIMAL;
    }

    VkFilter GetVulkanFilter(GraphicsFilter const& filter)
    {
        switch (filter)
        {
        case GraphicsFilter_Nearest:
            return VK_FILTER_NEAREST;
        case GraphicsFilter_Linear:
            return VK_FILTER_LINEAR;
        default:
            break;
        }

        return VK_FILTER_NEAREST;
    }

    VkSamplerMipmapMode GetVulkanSamplerMipmapMode(GraphicsSamplerMipmapMode const& mipmapMode)
    {
        switch (mipmapMode)
        {
        case GraphicsSamplerMipmapMode_Nearest:
            return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        case GraphicsSamplerMipmapMode_Linear:
            return VK_SAMPLER_MIPMAP_MODE_LINEAR;
        }

        return VK_SAMPLER_MIPMAP_MODE_NEAREST;
    }

    VkSamplerAddressMode GetVulkanSamplerAddressMode(GraphicsSamplerAddressMode const& addressMode)
    {
        switch (addressMode)
        {
        case GraphicsSamplerAddressMode_Repeat:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case GraphicsSamplerAddressMode_MirroredRepeat:
            return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case GraphicsSamplerAddressMode_ClampToEdge:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case GraphicsSamplerAddressMode_ClampToBorder:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        default:
            break;
        }

        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    }

    VkBorderColor GetVulkanBorderColor(GraphicsBorderColor const& borderColor)
    {
        switch (borderColor)
        {
        case GraphicsBorderColor_TransparentBlack_Float:
            return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        case GraphicsBorderColor_TransparentBlack_Int:
            return VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
        case GraphicsBorderColor_OpaqueBlack_Float:
            return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
        case GraphicsBorderColor_OpaqueBlack_Int:
            return VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        case GraphicsBorderColor_OpaqueWhite_Float:
            return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        case GraphicsBorderColor_OpaqueWhite_Int:
            return VK_BORDER_COLOR_INT_OPAQUE_WHITE;
        default:
            break;
        }

        return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    }

    VkCompareOp GetVulkanCompareOp(GraphicsCompareOp const& compareOp)
    {
        switch (compareOp)
        {
        case GraphicsCompareOp_Undefined:
        case GraphicsCompareOp_Never:
            return VK_COMPARE_OP_NEVER;
        case GraphicsCompareOp_Less:
            return VK_COMPARE_OP_LESS;
        case GraphicsCompareOp_Equal:
            return VK_COMPARE_OP_EQUAL;
        case GraphicsCompareOp_LessOrEqual:
            return VK_COMPARE_OP_LESS_OR_EQUAL;
        case GraphicsCompareOp_Greater:
            return VK_COMPARE_OP_GREATER;
        case GraphicsCompareOp_NotEqual:
            return VK_COMPARE_OP_NOT_EQUAL;
        case GraphicsCompareOp_GreaterOrEqual:
            return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case GraphicsCompareOp_Always:
            return VK_COMPARE_OP_ALWAYS;
        default:
            break;
        }

        return VK_COMPARE_OP_NEVER;
    }

    VkImageAspectFlags GetVulkanImageAspectFlags(GraphicsTextureAspectFlags const& aspectMask)
    {
        VkImageAspectFlags output = 0;
        if (IsBitSet(aspectMask, GraphicsTextureAspect_Color)) {
            output |= VK_IMAGE_ASPECT_COLOR_BIT;
        }
        if (IsBitSet(aspectMask, GraphicsTextureAspect_Depth)) {
            output |= VK_IMAGE_ASPECT_DEPTH_BIT;
        }
        if (IsBitSet(aspectMask, GraphicsTextureAspect_Stencil)) {
            output |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }

        return output;
    }

    VkBufferUsageFlags GetVulkanBufferUsageFlags(GraphicsBufferUsageFlags const& usageFlags)
    {
        VkBufferUsageFlags output = 0;
        if (IsBitSet(usageFlags, GraphicsBufferUsage_TransferSrc)) {
            output |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        }
        if (IsBitSet(usageFlags, GraphicsBufferUsage_TransferDst)) {
            output |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        }
        if (IsBitSet(usageFlags, GraphicsBufferUsage_UniformBuffer)) {
            output |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        }
        if (IsBitSet(usageFlags, GraphicsBufferUsage_StorageBuffer)) {
            output |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        }
        if (IsBitSet(usageFlags, GraphicsBufferUsage_IndexBuffer)) {
            output |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        }
        if (IsBitSet(usageFlags, GraphicsBufferUsage_VertexBuffer)) {
            output |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        }
        if (IsBitSet(usageFlags, GraphicsBufferUsage_IndirectBuffer)) {
            output |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        }

        return output;
    }

    VkImageUsageFlags GetVulkanImageUsageFlags(GraphicsTextureUsageFlags const& usageFlags)
    {
        VkImageUsageFlags output = 0;
        if (IsBitSet(usageFlags, GraphicsTextureUsage_TransferSrc)) {
            output |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        }
        if (IsBitSet(usageFlags, GraphicsTextureUsage_TransferDst)) {
            output |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        }
        if (IsBitSet(usageFlags, GraphicsTextureUsage_SampledImage)) {
            output |= VK_IMAGE_USAGE_SAMPLED_BIT;
        }
        if (IsBitSet(usageFlags, GraphicsTextureUsage_StorageImage)) {
            output |= VK_IMAGE_USAGE_STORAGE_BIT;
        }
        if (IsBitSet(usageFlags, GraphicsTextureUsage_RenderTarget)) {
            output |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }
        if (IsBitSet(usageFlags, GraphicsTextureUsage_DepthStencilTarget)) {
            output |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        }

        return output;
    }
} // namespace Bonsai::VulkanGraphicsHelpers
