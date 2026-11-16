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
        default:
            break;
        }

        return VK_FORMAT_UNDEFINED;
    }

    GraphicsFormat GetGraphicsFormat(VkFormat const& format)
    {
        switch (format)
        {
        case VK_FORMAT_UNDEFINED: return GraphicsFormat_Unknown;
        case VK_FORMAT_R8_UNORM: return GraphicsFormat_R8_UNorm;
        case VK_FORMAT_R8_SNORM: return GraphicsFormat_R8_SNorm;
        case VK_FORMAT_R8_UINT: return GraphicsFormat_R8_UInt;
        case VK_FORMAT_R8_SINT: return GraphicsFormat_R8_SInt;
        case VK_FORMAT_R8G8_UNORM: return GraphicsFormat_RG8_UNorm;
        case VK_FORMAT_R8G8_SNORM: return GraphicsFormat_RG8_SNorm;
        case VK_FORMAT_R8G8_UINT: return GraphicsFormat_RG8_UInt;
        case VK_FORMAT_R8G8_SINT: return GraphicsFormat_RG8_SInt;
        case VK_FORMAT_R8G8B8A8_UNORM: return GraphicsFormat_RGBA8_UNorm;
        case VK_FORMAT_R8G8B8A8_SNORM: return GraphicsFormat_RGBA8_SNorm;
        case VK_FORMAT_R8G8B8A8_UINT: return GraphicsFormat_RGBA8_UInt;
        case VK_FORMAT_R8G8B8A8_SINT: return GraphicsFormat_RGBA8_SInt;
        case VK_FORMAT_R8G8B8A8_SRGB: return GraphicsFormat_RGBA8_UNorm_SRGB;
        case VK_FORMAT_B8G8R8_UNORM: return GraphicsFormat_BGRA8_UNorm;
        case VK_FORMAT_B8G8R8_SRGB: return GraphicsFormat_BGRA8_UNorm_SRGB;
        case VK_FORMAT_R16_UNORM: return GraphicsFormat_R16_UNorm;
        case VK_FORMAT_R16_SNORM: return GraphicsFormat_R16_SNorm;
        case VK_FORMAT_R16_UINT: return GraphicsFormat_R16_UInt;
        case VK_FORMAT_R16_SINT: return GraphicsFormat_R16_SInt;
        case VK_FORMAT_R16_SFLOAT: return GraphicsFormat_R16_SFloat;
        case VK_FORMAT_R16G16_UNORM: return GraphicsFormat_RG16_UNorm;
        case VK_FORMAT_R16G16_SNORM: return GraphicsFormat_RG16_SNorm;
        case VK_FORMAT_R16G16_UINT: return GraphicsFormat_RG16_UInt;
        case VK_FORMAT_R16G16_SINT: return GraphicsFormat_RG16_SInt;
        case VK_FORMAT_R16G16_SFLOAT: return GraphicsFormat_RG16_SFloat;
        case VK_FORMAT_R16G16B16A16_UNORM: return GraphicsFormat_RGBA16_UNorm;
        case VK_FORMAT_R16G16B16A16_SNORM: return GraphicsFormat_RGBA16_SNorm;
        case VK_FORMAT_R16G16B16A16_UINT: return GraphicsFormat_RGBA16_UInt;
        case VK_FORMAT_R16G16B16A16_SINT: return GraphicsFormat_RGBA16_SInt;
        case VK_FORMAT_R16G16B16A16_SFLOAT: return GraphicsFormat_RGBA16_SFloat;
        case VK_FORMAT_R32_UINT: return GraphicsFormat_R32_UInt;
        case VK_FORMAT_R32_SINT: return GraphicsFormat_R32_SInt;
        case VK_FORMAT_R32_SFLOAT: return GraphicsFormat_R32_SFloat;
        case VK_FORMAT_R32G32_UINT: return GraphicsFormat_RG32_UInt;
        case VK_FORMAT_R32G32_SINT: return GraphicsFormat_RG32_SInt;
        case VK_FORMAT_R32G32_SFLOAT: return GraphicsFormat_RG32_SFloat;
        case VK_FORMAT_R32G32B32_UINT: return GraphicsFormat_RGB32_UInt;
        case VK_FORMAT_R32G32B32_SINT: return GraphicsFormat_RGB32_SInt;
        case VK_FORMAT_R32G32B32_SFLOAT: return GraphicsFormat_RGB32_SFloat;
        case VK_FORMAT_R32G32B32A32_UINT: return GraphicsFormat_RGBA32_UInt;
        case VK_FORMAT_R32G32B32A32_SINT: return GraphicsFormat_RGBA32_SInt;
        case VK_FORMAT_R32G32B32A32_SFLOAT: return GraphicsFormat_RGBA32_SFloat;
        case VK_FORMAT_D16_UNORM: return GraphicsFormat_D16_UNorm;
        case VK_FORMAT_D24_UNORM_S8_UINT: return GraphicsFormat_D24_UNorm_S8_UInt;
        case VK_FORMAT_D32_SFLOAT: return GraphicsFormat_D32_SFloat;
        default:
            break;
        }

        return GraphicsFormat_Unknown;
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

    VkImageLayout GetVulkanImageLayout(GraphicsTextureLayout const& textureLayout)
    {
        switch (textureLayout)
        {
        case GraphicsTextureLayout_Undefined:
            return VK_IMAGE_LAYOUT_UNDEFINED;      
        case GraphicsTextureLayout_General:
            return VK_IMAGE_LAYOUT_GENERAL;
        case GraphicsTextureLayout_RenderTarget:
            return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        case GraphicsTextureLayout_DepthStencilTarget:
            return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        case GraphicsTextureLayout_DepthStencilReadOnlyTarget:
            return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
        case GraphicsTextureLayout_ShaderReadOnly:
            return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        case GraphicsTextureLayout_TransferSrc:
            return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        case GraphicsTextureLayout_TransferDst:
            return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        case GraphicsTextureLayout_Present:
            return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        default:
            break;
        }

        return VK_IMAGE_LAYOUT_UNDEFINED;
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

    VkDescriptorType GetVulkanDescriptorType(GraphicsDescriptorType const& descriptorType)
    {
        switch (descriptorType)
        {
        case GraphicsDescriptorType_Sampler:
            return VK_DESCRIPTOR_TYPE_SAMPLER;
        case GraphicsDescriptorType_CombinedImageSampler:
            return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        case GraphicsDescriptorType_SampledImage:
            return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        case GraphicsDescriptorType_StorageImage:
            return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        case GraphicsDescriptorType_UniformBuffer:
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case GraphicsDescriptorType_StorageBuffer:
            return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        default:
            break;
        }

        return VK_DESCRIPTOR_TYPE_SAMPLER;
    }

    VkVertexInputRate GetVulkanVertexInputRate(GraphicsVertexInputRate const& inputRate)
    {
        switch (inputRate)
        {
        case GraphicsVertexInputRate_PerVertex:
            return VK_VERTEX_INPUT_RATE_VERTEX;
        case GraphicsVertexInputRate_PerInstance:
            return VK_VERTEX_INPUT_RATE_INSTANCE;
        default:
            break;
        }

        return VK_VERTEX_INPUT_RATE_VERTEX;
    }

    VkPrimitiveTopology GetVulkanPrimitiveTopology(GraphicsPrimitiveTopology const& primitiveTopology)
    {
        switch (primitiveTopology)
        {
        case GraphicsPrimitiveTopology_PointList:
            return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        case GraphicsPrimitiveTopology_LineList:
            return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        case GraphicsPrimitiveTopology_LineStrip:
            return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
        case GraphicsPrimitiveTopology_TriangleList:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        case GraphicsPrimitiveTopology_TriangleStrip:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        case GraphicsPrimitiveTopology_TriangleFan:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
        case GraphicsPrimitiveTopology_LineListWithAdjacency:
            return VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
        case GraphicsPrimitiveTopology_LineStripWithAdjacency:
            return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
        case GraphicsPrimitiveTopology_TriangleListWithAdjacency:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
        case GraphicsPrimitiveTopology_TriangleStripWithAdjacency:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
        default:
            break;
        }

        return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    }

    VkPolygonMode GetVulkanPolygonMode(GraphicsPolygonMode const& polygonMode)
    {
        switch (polygonMode)
        {
        case GraphicsPolygonMode_Fill:
            return VK_POLYGON_MODE_FILL;
        case GraphicsPolygonMode_Line:
            return VK_POLYGON_MODE_LINE;
        case GraphicsPolygonMode_Point:
            return VK_POLYGON_MODE_POINT;
        default:
            break;
        }

        return VK_POLYGON_MODE_FILL;
    }

    VkCullModeFlags GetVulkanCullModeFlags(GraphicsCullMode const& cullMode)
    {
        switch (cullMode)
        {
        case GraphicsCullMode_None:
            return VK_CULL_MODE_NONE;
        case GraphicsCullMode_Front:
            return VK_CULL_MODE_FRONT_BIT;
        case GraphicsCullMode_Back:
            return VK_CULL_MODE_BACK_BIT;
        case GraphicsCullMode_FrontAndBack:
            return VK_CULL_MODE_FRONT_AND_BACK;
        default:
            break;
        }

        return VK_CULL_MODE_NONE;
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

    VkLogicOp GetVulkanLogicOp(GraphicsLogicOp const& logicOp)
    {
        switch (logicOp)
        {
        case GraphicsLogicOp_Undefined:
        case GraphicsLogicOp_Clear:
            return VK_LOGIC_OP_CLEAR;
        case GraphicsLogicOp_And:
            return VK_LOGIC_OP_AND;
        case GraphicsLogicOp_AndReverse:
            return VK_LOGIC_OP_AND_REVERSE;
        case GraphicsLogicOp_Copy:
            return VK_LOGIC_OP_COPY;
        case GraphicsLogicOp_AndInverted:
            return VK_LOGIC_OP_AND_INVERTED;
        case GraphicsLogicOp_NoOp:
            return VK_LOGIC_OP_NO_OP;
        case GraphicsLogicOp_Xor:
            return VK_LOGIC_OP_XOR;
        case GraphicsLogicOp_Or:
            return VK_LOGIC_OP_OR;
        case GraphicsLogicOp_Nor:
            return VK_LOGIC_OP_NOR;
        case GraphicsLogicOp_Equivalent:
            return VK_LOGIC_OP_EQUIVALENT;
        case GraphicsLogicOp_Invert:
            return VK_LOGIC_OP_INVERT;
        case GraphicsLogicOp_OrReverse:
            return VK_LOGIC_OP_OR_REVERSE;
        case GraphicsLogicOp_CopyInverted:
            return VK_LOGIC_OP_COPY_INVERTED;
        case GraphicsLogicOp_OrInverted:
            return VK_LOGIC_OP_OR_INVERTED;
        case GraphicsLogicOp_Nand:
            return VK_LOGIC_OP_NAND;
        case GraphicsLogicOp_Set:
            return VK_LOGIC_OP_SET;
        default:
            break;
        }

        return VK_LOGIC_OP_CLEAR;
    }

    VkStencilOp GetVulkanStencilOp(GraphicsStencilOp const& stencilOp)
    {
        switch (stencilOp)
        {
        case GraphicsStencilOp_Keep:
            return VK_STENCIL_OP_KEEP;
        case GraphicsStencilOp_Zero:
            return VK_STENCIL_OP_ZERO;
        case GraphicsStencilOp_Replace:
            return VK_STENCIL_OP_REPLACE;
        case GraphicsStencilOp_IncrementClamp:
            return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
        case GraphicsStencilOp_DecrementClamp:
            return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
        case GraphicsStencilOp_Invert:
            return VK_STENCIL_OP_INVERT;
        case GraphicsStencilOp_IncrementWrap:
            return VK_STENCIL_OP_INCREMENT_AND_WRAP;
        case GraphicsStencilOp_DecrementWrap:
            return VK_STENCIL_OP_DECREMENT_AND_WRAP;
        default:
            break;
        }

        return VK_STENCIL_OP_KEEP;
    }

    VkBlendOp GetVulkanBlendOp(GraphicsBlendOp const& blendOp)
    {
        switch (blendOp)
        {
        case GraphicsBlendOp_Add:
            return VK_BLEND_OP_ADD;
        case GraphicsBlendOp_Subtract:
            return VK_BLEND_OP_SUBTRACT;
        case GraphicsBlendOp_ReverseSubtract:
            return VK_BLEND_OP_REVERSE_SUBTRACT;
        case GraphicsBlendOp_Min:
            return VK_BLEND_OP_MIN;
        case GraphicsBlendOp_Max:
            return VK_BLEND_OP_MAX;
        default:
            break;
        }

        return VK_BLEND_OP_ADD;
    }

    VkBlendFactor GetVulkanBlendFactor(GraphicsBlendFactor const& blendFactor)
    {
        switch (blendFactor)
        {
        case GraphicsBlendFactor_Zero:
            return VK_BLEND_FACTOR_ZERO;
        case GraphicsBlendFactor_One:
            return VK_BLEND_FACTOR_ONE;
        case GraphicsBlendFactor_SrcColor:
            return VK_BLEND_FACTOR_SRC_COLOR;
        case GraphicsBlendFactor_OneMinusSrcColor:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        case GraphicsBlendFactor_DstColor:
            return VK_BLEND_FACTOR_DST_COLOR;
        case GraphicsBlendFactor_OneMinusDstColor:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case GraphicsBlendFactor_SrcAlpha:
            return VK_BLEND_FACTOR_SRC_ALPHA;
        case GraphicsBlendFactor_OneMinusSrcAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case GraphicsBlendFactor_DstAlpha:
            return VK_BLEND_FACTOR_DST_ALPHA;
        case GraphicsBlendFactor_OneMinusDstAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        case GraphicsBlendFactor_ConstantColor:
            return VK_BLEND_FACTOR_CONSTANT_COLOR;
        case GraphicsBlendFactor_OneMinusConstantColor:
            return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
        case GraphicsBlendFactor_ConstantAlpha:
            return VK_BLEND_FACTOR_CONSTANT_ALPHA;
        case GraphicsBlendFactor_OneMinusConstantAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
        case GraphicsBlendFactor_SrcAlphaSaturate:
            return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
        case GraphicsBlendFactor_Src1Color:
            return VK_BLEND_FACTOR_SRC1_COLOR;
        case GraphicsBlendFactor_OneMinusSrc1Color:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
        case GraphicsBlendFactor_Src1Alpha:
            return VK_BLEND_FACTOR_SRC1_ALPHA;
        case GraphicsBlendFactor_OneMinusSrc1Alpha:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
        default:
            break;
        }

        return VK_BLEND_FACTOR_ZERO;
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

    VkShaderStageFlags GetVulkanShaderStageFlags(GraphicsShaderStageFlags const& stageFlags)
    {
        VkShaderStageFlags output = 0;
        if (IsBitSet(stageFlags, GraphicsShaderStage_Vertex)) {
            output |= VK_SHADER_STAGE_VERTEX_BIT;
        }
        if (IsBitSet(stageFlags, GraphicsShaderStage_TessControl)) {
            output |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        }
        if (IsBitSet(stageFlags, GraphicsShaderStage_TessEval)) {
            output |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        }
        if (IsBitSet(stageFlags, GraphicsShaderStage_Fragment)) {
            output |= VK_SHADER_STAGE_FRAGMENT_BIT;
        }
        if (IsBitSet(stageFlags, GraphicsShaderStage_Compute)) {
            output |= VK_SHADER_STAGE_COMPUTE_BIT;
        }

        return output;
    }

    VkColorComponentFlags GetVulkanColorComponentFlags(GraphicsColorComponentFlags const& colorComponentFlags)
    {
        VkColorComponentFlags output = 0;
        if (IsBitSet(colorComponentFlags, GraphicsColorComponent_Red)) {
            output |= VK_COLOR_COMPONENT_R_BIT;
        }
        if (IsBitSet(colorComponentFlags, GraphicsColorComponent_Green)) {
            output |= VK_COLOR_COMPONENT_G_BIT;
        }
        if (IsBitSet(colorComponentFlags, GraphicsColorComponent_Blue)) {
            output |= VK_COLOR_COMPONENT_B_BIT;
        }
        if (IsBitSet(colorComponentFlags, GraphicsColorComponent_Alpha)) {
            output |= VK_COLOR_COMPONENT_A_BIT;
        }

        return output;
    }
} // namespace Bonsai::VulkanGraphicsHelpers
