#pragma once
#ifndef BONSAI_GRAPHICS_TYPES_HPP
#define BONSAI_GRAPHICS_TYPES_HPP

#include <cstddef>
#include <cstdint>
#include <SDL3/SDL.h>

namespace Bonsai
{
    class GraphicsBuffer;
    class GraphicsTexture;
    class GraphicsTextureView;
    class GraphicsSampler;
    class GraphicsDescriptorSetLayout;
    class GraphicsDescriptorSet;
    class GraphicsPipelineLayout;
    class GraphicsRenderPipeline;
    class GraphicsComputePipeline;
    class GraphicsSwapchain;
    class GraphicsCommandBuffer;
    class FrameContext;
    class TransferBatch;

    namespace Graphics
    {
        static constexpr uint32_t   MaxRenderTargets                            = 8u;
        static constexpr uint32_t   MaxVertexInputBindings                      = 32u;
        static constexpr uint32_t   MaxVertexInputAttributes                    = 32u;
        static constexpr uint32_t   MaxPushConstantsSize                        = 256u;
        static constexpr uint32_t   MaxBoundDescriptorSets                      = 8u;
        static constexpr uint32_t   MaxDescriptorSetBindings                    = 32u;
        static constexpr uint32_t   MaxPushConstantRanges                       = 8u;
        static constexpr uint32_t   MaxSamplerAnisotropy                        = 16u;
        static constexpr uint32_t   MinUniformBufferOffsetAlignment             = 64u;
        static constexpr uint32_t   MinStorageBufferOffsetAlignment             = 16u;
        static constexpr uint32_t   MaxGlobalBarriers                           = 16u;
        static constexpr uint32_t   MaxBufferBarriers                           = 16u;
        static constexpr uint32_t   MaxTextureBarriers                          = 16u;
        static constexpr uint32_t   MaxCommandBuffersPerFrame                   = 32u;

        static constexpr uint32_t   WholeSize                                   = UINT32_MAX;
        static constexpr uint32_t   RemainingMipLevels                          = UINT32_MAX;
        static constexpr uint32_t   RemainingArrayLayers                        = UINT32_MAX;
        static constexpr float      LODClampNone                                = 1'000.0F;
    } // namespace Graphics

    /// @brief Enumeration of available graphics heap types.
    enum GraphicsHeapType : uint32_t
    {
        GraphicsHeapType_Default    = 0,
        GraphicsHeapType_Upload     = 1,
        GraphicsHeapType_ReadBack   = 2,
    };

    /// @brief Enumeration of available graphics index types.
    enum GraphicsIndexType : uint32_t
    {
        GraphicsIndexType_UInt16    = 0,
        GraphicsIndexType_UInt32    = 1,
    };

    /// @brief Enumeration of available graphics load ops.
    enum GraphicsLoadOp : uint32_t
    {
        GraphicsLoadOp_Load     = 0,
        GraphicsLoadOp_Clear    = 1,
        GraphicsLoadOp_DontCare = 2,
    };

    /// @brief Enumeration of available graphics store ops.
    enum GraphicsStoreOp : uint32_t
    {
        GraphicsStoreOp_Store       = 0,
        GraphicsStoreOp_DontCare    = 1,
    };

    /// @brief Enumeration of available graphics formats.
    /// Used for texture formats, vertex binding layouts, etc.
    enum GraphicsFormat : uint32_t
    {
        GraphicsFormat_Unknown = 0,

        GraphicsFormat_R8_UNorm,
        GraphicsFormat_R8_SNorm,
        GraphicsFormat_R8_UInt,
        GraphicsFormat_R8_SInt,

        GraphicsFormat_RG8_UNorm,
        GraphicsFormat_RG8_SNorm,
        GraphicsFormat_RG8_UInt,
        GraphicsFormat_RG8_SInt,

        GraphicsFormat_RGBA8_UNorm,
        GraphicsFormat_RGBA8_SNorm,
        GraphicsFormat_RGBA8_UInt,
        GraphicsFormat_RGBA8_SInt,
        GraphicsFormat_RGBA8_UNorm_SRGB,

        GraphicsFormat_BGRA8_UNorm,
        GraphicsFormat_BGRA8_UNorm_SRGB,

        GraphicsFormat_R16_UNorm,
        GraphicsFormat_R16_SNorm,
        GraphicsFormat_R16_UInt,
        GraphicsFormat_R16_SInt,
        GraphicsFormat_R16_SFloat,

        GraphicsFormat_RG16_UNorm,
        GraphicsFormat_RG16_SNorm,
        GraphicsFormat_RG16_UInt,
        GraphicsFormat_RG16_SInt,
        GraphicsFormat_RG16_SFloat,
        
        GraphicsFormat_RGBA16_UNorm,
        GraphicsFormat_RGBA16_SNorm,
        GraphicsFormat_RGBA16_UInt,
        GraphicsFormat_RGBA16_SInt,
        GraphicsFormat_RGBA16_SFloat,

        GraphicsFormat_R32_UInt,
        GraphicsFormat_R32_SInt,
        GraphicsFormat_R32_SFloat,

        GraphicsFormat_RG32_UInt,
        GraphicsFormat_RG32_SInt,
        GraphicsFormat_RG32_SFloat,
        
        GraphicsFormat_RGB32_UInt,
        GraphicsFormat_RGB32_SInt,
        GraphicsFormat_RGB32_SFloat,

        GraphicsFormat_RGBA32_UInt,
        GraphicsFormat_RGBA32_SInt,
        GraphicsFormat_RGBA32_SFloat,

        GraphicsFormat_D16_UNorm,
        GraphicsFormat_D24_UNorm_S8_UInt,
        GraphicsFormat_D32_SFloat,
    };

    /// @brief Enumeration of available graphics texture types.
    enum GraphicsTextureType : uint32_t
    {
        GraphicsTextureType_1D      = 0,
        GraphicsTextureType_2D      = 1,
        GraphicsTextureType_3D      = 2,
    };

    /// @brief Enumeration of available graphics texture view types.
    enum GraphicsTextureViewType : uint32_t
    {
        GraphicsTextureViewType_1D      = 0,
        GraphicsTextureViewType_2D      = 1,
        GraphicsTextureViewType_3D      = 2,
        GraphicsTextureViewType_Cube    = 3,
        GraphicsTextureViewType_1DArray = 4,
        GraphicsTextureViewType_2DArray = 5,
    };

    /// @brief Enumeration of available graphics texture tiling modes.
    enum GraphicsTextureTiling : uint32_t
    {
        GraphicsTextureTiling_Optimal   = 0,
        GraphicsTextureTiling_Linear    = 1,
    };

    /// @brief Enumeration of available graphics filters.
    enum GraphicsFilter : uint32_t
    {
        GraphicsFilter_Nearest  = 0,
        GraphicsFilter_Linear   = 1,
    };

    /// @brief Enumeration of available graphics mipmap modes.
    enum GraphicsSamplerMipmapMode : uint32_t
    {
        GraphicsSamplerMipmapMode_Nearest  = 0,
        GraphicsSamplerMipmapMode_Linear   = 1,
    };

    /// @brief Enumeration of available graphics sampler address modes.
    enum GraphicsSamplerAddressMode : uint32_t
    {
        GraphicsSamplerAddressMode_Repeat           = 0,
        GraphicsSamplerAddressMode_MirroredRepeat   = 1,
        GraphicsSamplerAddressMode_ClampToEdge      = 2,
        GraphicsSamplerAddressMode_ClampToBorder    = 3,
    };

    /// @brief Enumeration of available graphics border colors.
    enum GraphicsBorderColor : uint32_t
    {
        GraphicsBorderColor_TransparentBlack_Float  = 0,
        GraphicsBorderColor_TransparentBlack_Int    = 1,
        GraphicsBorderColor_OpaqueBlack_Float       = 2,
        GraphicsBorderColor_OpaqueBlack_Int         = 3,
        GraphicsBorderColor_OpaqueWhite_Float       = 4,
        GraphicsBorderColor_OpaqueWhite_Int         = 5,
    };

    /// @brief Enumeration of available graphics descriptor types.
    enum GraphicsDescriptorType : uint32_t
    {
        GraphicsDescriptorType_Sampler                  = 0,
        GraphicsDescriptorType_CombinedImageSampler     = 1,
        GraphicsDescriptorType_SampledImage             = 2,
        GraphicsDescriptorType_StorageImage             = 3,
        GraphicsDescriptorType_UniformBuffer            = 4,
        GraphicsDescriptorType_StorageBuffer            = 5,
    };

    /// @brief Enumeration of available graphics vertex input rates.
    enum GraphicsVertexInputRate : uint32_t
    {
        GraphicsVertexInputRate_PerVertex   = 0,
        GraphicsVertexInputRate_PerInstance = 1,
    };

    /// @brief Enumeration of available graphics primitive topology types.
    enum GraphicsPrimitiveTopology : uint32_t
    {
        GraphicsPrimitiveTopology_PointList                     = 0,
        GraphicsPrimitiveTopology_LineList                      = 1,
        GraphicsPrimitiveTopology_LineStrip                     = 2,
        GraphicsPrimitiveTopology_TriangleList                  = 3,
        GraphicsPrimitiveTopology_TriangleStrip                 = 4,
        GraphicsPrimitiveTopology_TriangleFan                   = 5,
        GraphicsPrimitiveTopology_LineListWithAdjacency         = 6,
        GraphicsPrimitiveTopology_LineStripWithAdjacency        = 7,
        GraphicsPrimitiveTopology_TriangleListWithAdjacency     = 8,
        GraphicsPrimitiveTopology_TriangleStripWithAdjacency    = 9,
        GraphicsPrimitiveTopology_PatchList                     = 10,
    };

    /// @brief Enumeration of available graphics index buffer strip cut values.
    enum GraphicsIndexBufferStripCutValue : uint32_t
    {
        GraphicsIndexBufferStripCutValue_None   = 0x00000000,
        GraphicsIndexBufferStripCutValue_UInt16 = 0x0000FFFF,
        GraphicsIndexBufferStripCutValue_UInt32 = 0xFFFFFFFF,
    };

    /// @brief Enumeration of available graphics polygon modes.
    enum GraphicsPolygonMode : uint32_t
    {
        GraphicsPolygonMode_Fill    = 0,
        GraphicsPolygonMode_Line    = 1,
        GraphicsPolygonMode_Point   = 2,
    };

    /// @brief Enumeration of available graphics cull modes.
    enum GraphicsCullMode : uint32_t
    {
        GraphicsCullMode_None           = 0,
        GraphicsCullMode_Front          = 1,
        GraphicsCullMode_Back           = 2,
        GraphicsCullMode_FrontAndBack   = 3,
    };

    /// @brief Enumeration of available graphics compare ops.
    enum GraphicsCompareOp : uint32_t
    {
        GraphicsCompareOp_Undefined         = 0,
        GraphicsCompareOp_Never             = 1,
        GraphicsCompareOp_Less              = 2,
        GraphicsCompareOp_Equal             = 3,
        GraphicsCompareOp_LessOrEqual       = 4,
        GraphicsCompareOp_Greater           = 5,
        GraphicsCompareOp_NotEqual          = 6,
        GraphicsCompareOp_GreaterOrEqual    = 7,
        GraphicsCompareOp_Always            = 8,
    };

    /// @brief Enumeration of available graphics logic ops.
    enum GraphicsLogicOp : uint32_t
    {
        GraphicsLogicOp_Undefined       = 0,
        GraphicsLogicOp_Clear           = 1,
        GraphicsLogicOp_And             = 2,
        GraphicsLogicOp_AndReverse      = 3,
        GraphicsLogicOp_Copy            = 4,
        GraphicsLogicOp_AndInverted     = 5,
        GraphicsLogicOp_NoOp            = 6,
        GraphicsLogicOp_Xor             = 7,
        GraphicsLogicOp_Or              = 8,
        GraphicsLogicOp_Nor             = 9,
        GraphicsLogicOp_Equivalent      = 10,
        GraphicsLogicOp_Invert          = 11,
        GraphicsLogicOp_OrReverse       = 12,
        GraphicsLogicOp_CopyInverted    = 13,
        GraphicsLogicOp_OrInverted      = 14,
        GraphicsLogicOp_Nand            = 15,
        GraphicsLogicOp_Set             = 16,
    };

    /// @brief Enumeration of available graphics stencil ops.
    enum GraphicsStencilOp : uint32_t
    {
        GraphicsStencilOp_Keep              = 0,
        GraphicsStencilOp_Zero              = 1,
        GraphicsStencilOp_Replace           = 2,
        GraphicsStencilOp_IncrementClamp    = 3,
        GraphicsStencilOp_DecrementClamp    = 4,
        GraphicsStencilOp_Invert            = 5,
        GraphicsStencilOp_IncrementWrap     = 6,
        GraphicsStencilOp_DecrementWrap     = 7,
    };

    /// @brief Enumeration of available graphics blend ops.
    enum GraphicsBlendOp : uint32_t
    {
        GraphicsBlendOp_Add             = 0,
        GraphicsBlendOp_Subtract        = 1,
        GraphicsBlendOp_ReverseSubtract = 2,
        GraphicsBlendOp_Min             = 3,
        GraphicsBlendOp_Max             = 4,
    };

    /// @brief Enumeration of available graphics blend factors.
    enum GraphicsBlendFactor : uint32_t
    {
        GraphicsBlendFactor_Zero                    = 0,
        GraphicsBlendFactor_One                     = 1,
        GraphicsBlendFactor_SrcColor                = 2,
        GraphicsBlendFactor_OneMinusSrcColor        = 3,
        GraphicsBlendFactor_DstColor                = 4,
        GraphicsBlendFactor_OneMinusDstColor        = 5,
        GraphicsBlendFactor_SrcAlpha                = 6,
        GraphicsBlendFactor_OneMinusSrcAlpha        = 7,
        GraphicsBlendFactor_DstAlpha                = 8,
        GraphicsBlendFactor_OneMinusDstAlpha        = 9,
        GraphicsBlendFactor_ConstantColor           = 10,
        GraphicsBlendFactor_OneMinusConstantColor   = 11,
        GraphicsBlendFactor_ConstantAlpha           = 12,
        GraphicsBlendFactor_OneMinusConstantAlpha   = 13,
        GraphicsBlendFactor_SrcAlphaSaturate        = 14,
        GraphicsBlendFactor_Src1Color               = 15,
        GraphicsBlendFactor_OneMinusSrc1Color       = 16,
        GraphicsBlendFactor_Src1Alpha               = 17,
        GraphicsBlendFactor_OneMinusSrc1Alpha       = 18,
    };

    /// @brief Enumeration of available graphics texture layouts.
    enum GraphicsTextureLayout : uint32_t
    {
        GraphicsTextureLayout_Undefined                     = 0,
        GraphicsTextureLayout_General                       = 1,
        GraphicsTextureLayout_RenderTarget                  = 2,
        GraphicsTextureLayout_DepthStencilTarget            = 3,
        GraphicsTextureLayout_DepthStencilReadOnlyTarget    = 4,
        GraphicsTextureLayout_ShaderReadOnly                = 5,
        GraphicsTextureLayout_TransferSrc                   = 6,
        GraphicsTextureLayout_TransferDst                   = 7,
        GraphicsTextureLayout_Present                       = 8,
    };

    /// @brief Enumeration of available graphics resource state.
    enum GraphicsResourceState : uint32_t
    {
        GraphicsResourceState_Common                    = 0,
        GraphicsResourceState_IndirectBuffer_Read       = 1,
        GraphicsResourceState_IndexBuffer_Read          = 2,
        GraphicsResourceState_VertexBuffer_Read         = 3,
        GraphicsResourceState_VertexShader_Read         = 4,
        GraphicsResourceState_VertexShader_Write        = 5,
        GraphicsResourceState_PixelShader_Read          = 6,
        GraphicsResourceState_PixelShader_Write         = 7,
        GraphicsResourceState_ComputeShader_Read        = 8,
        GraphicsResourceState_ComputeShader_Write       = 9,
        GraphicsResourceState_RenderTarget_Read         = 10,
        GraphicsResourceState_RenderTarget_Write        = 11,
        GraphicsResourceState_DepthStencilTarget_Read   = 12,
        GraphicsResourceState_DepthStencilTarget_Write  = 13,
        GraphicsResourceState_Transfer_Read             = 14,
        GraphicsResourceState_Transfer_Write            = 15,
        GraphicsResourceState_Host_Read                 = 16,
        GraphicsResourceState_Host_Write                = 17,
        GraphicsResourceState_Present                   = 18,
    };

    /// @brief Enumeration of graphics texture aspect flag bits.
    enum GraphicsTextureAspectFlagBits : uint32_t
    {
        GraphicsTextureAspect_None      = 0x00000000,
        GraphicsTextureAspect_Color     = 0x00000001,
        GraphicsTextureAspect_Depth     = 0x00000002,
        GraphicsTextureAspect_Stencil   = 0x00000004,
    };
    using GraphicsTextureAspectFlags = uint32_t;

    /// @brief Enumeration of graphics buffer usage flag bits.
    enum GraphicsBufferUsageFlagBits : uint32_t
    {
        GraphicsBufferUsage_None            = 0x00000000,
        GraphicsBufferUsage_TransferSrc     = 0x00000001,
        GraphicsBufferUsage_TransferDst     = 0x00000002,
        GraphicsBufferUsage_UniformBuffer   = 0x00000004,
        GraphicsBufferUsage_StorageBuffer   = 0x00000008,
        GraphicsBufferUsage_IndexBuffer     = 0x00000016,
        GraphicsBufferUsage_VertexBuffer    = 0x00000032,
        GraphicsBufferUsage_IndirectBuffer  = 0x00000064,
    };
    using GraphicsBufferUsageFlags = uint32_t;

    /// @brief Enumeration of graphics texture usage flag bits.
    enum GraphicsTextureUsageFlagBits : uint32_t
    {
        GraphicsTextureUsage_None               = 0x00000000,
        GraphicsTextureUsage_TransferSrc        = 0x00000001,
        GraphicsTextureUsage_TransferDst        = 0x00000002,
        GraphicsTextureUsage_SampledImage       = 0x00000004,
        GraphicsTextureUsage_StorageImage       = 0x00000008,
        GraphicsTextureUsage_RenderTarget       = 0x00000010,
        GraphicsTextureUsage_DepthStencilTarget = 0x00000020,
    };
    using GraphicsTextureUsageFlags = uint32_t;

    /// @brief Enumeration of graphics shader stage flag bits.
    enum GraphicsShaderStageFlagBits : uint32_t
    {
        GraphicsShaderStage_None        = 0x00000000,
        GraphicsShaderStage_Vertex      = 0x00000001,
        GraphicsShaderStage_TessControl = 0x00000002,
        GraphicsShaderStage_TessEval    = 0x00000004,
        GraphicsShaderStage_Fragment    = 0x00000008,
        GraphicsShaderStage_Compute     = 0x00000010,
        GraphicsShaderStage_AllGraphics = GraphicsShaderStage_Vertex | GraphicsShaderStage_TessControl | GraphicsShaderStage_TessEval | GraphicsShaderStage_Fragment,
    };
    using GraphicsShaderStageFlags = uint32_t;

    /// @brief Enumeration of graphics color write flag bits.
    enum GraphicsColorComponentFlagBits : uint32_t
    {
        GraphicsColorComponent_None     = 0x00000000,
        GraphicsColorComponent_Red      = 0x00000001,
        GraphicsColorComponent_Green    = 0x00000002,
        GraphicsColorComponent_Blue     = 0x00000004,
        GraphicsColorComponent_Alpha    = 0x00000008,
        GraphicsColorComponent_All      = 0xFFFFFFFF,
    };
    using GraphicsColorComponentFlags = uint32_t;

    /// @brief 2D offset into a resource.
    struct Offset2D
    {
        int32_t x;
        int32_t y;
    };

    /// @brief 3D offset into a resource.
    struct Offset3D
    {
        int32_t x;
        int32_t y;
        int32_t z;
    };

    /// @brief 2D extent of a resource.
    struct Extent2D
    {
        uint32_t width;
        uint32_t height;
    };

    /// @brief 3D extent of a resource.
    struct Extent3D
    {
        uint32_t width;
        uint32_t height;
        uint32_t depth;
    };

    /// @brief 2D rectangle.
    struct Rect2D
    {
        Offset2D offset;
        Extent2D extent;
    };

    /// @brief Viewport for rasterization pipelines.
    struct GraphicsViewport
    {
        float x;
        float y;
        float width;
        float height;
        float minDepth;
        float maxDepth;
    };

    /// @brief Clear value for color targets.
    union GraphicsClearColorValue
    {
        float float32[4];
        int32_t int32[4];
        uint32_t uint32[4];
    };

    /// @brief Clear value for depth stencil targets.
    struct GraphicsClearDepthStencilValue
    {
        float depth;
        uint32_t stencil;
    };

    /// @brief Clear value for textures.
    union GraphicsClearValue
    {
        GraphicsClearColorValue color;
        GraphicsClearDepthStencilValue depthStencil;
    };

    /// @brief Push constant range for shader pipeline layouts.
    struct GraphicsPushConstantRange
    {
        GraphicsShaderStageFlags stages;
        size_t offset;
        size_t size;
    };

    /// @brief Binding element for descriptor set layouts.
    struct GraphicsDescriptorSetLayoutBinding
    {
        uint32_t binding;
        GraphicsDescriptorType descriptorType;
        uint32_t descriptorCount;
        GraphicsShaderStageFlags stages;
        GraphicsSampler* const* staticSamplers;
    };

    /// @brief Graphics descriptor buffer info.
    struct GraphicsDescriptorBufferInfo
    {
        GraphicsBuffer* buffer;
        size_t offset;
        size_t range;
    };

    /// @brief Graphics descriptor image info.
    struct GraphicsDescriptorImageInfo
    {
        GraphicsSampler* sampler;
        GraphicsTextureView* textureView;
        GraphicsTextureLayout textureLayout;
    };

    /// @brief Shader entrypoint for shader pipelines.
    struct GraphicsShaderEntrypoint
    {
        char const* name;
        size_t size;
    };

    /// @brief Shader bytecode for shader pipelines.
    struct GraphicsShaderBytecode
    {
        void const* code;
        size_t size;
    };

    /// @brief Vertex binding for the vertex input state.
    struct GraphicsVertexBinding
    {
        uint32_t binding;
        uint32_t stride;
        GraphicsVertexInputRate inputRate;
    };

    /// @brief Vertex attribute for a vertex binding.
    struct GraphicsVertexAttribute
    {
        uint32_t location;
        uint32_t binding;
        GraphicsFormat format;
        uint32_t offset;
    };

    /// @brief Stencil op state for stencil tests.
    struct GraphicsStencilOpState
    {
        GraphicsStencilOp failOp = GraphicsStencilOp_Keep;
        GraphicsStencilOp passOp = GraphicsStencilOp_Keep;
        GraphicsStencilOp depthFailOp = GraphicsStencilOp_Keep;
        GraphicsCompareOp compareOp = GraphicsCompareOp_Never;
    };

    /// @brief Blend state for render target color blending.
    struct GraphicsRenderTargetBlendState
    {
        bool blendEnable = false;
        GraphicsBlendFactor srcColorBlendFactor = GraphicsBlendFactor_Zero;
        GraphicsBlendFactor dstColorBlendFactor = GraphicsBlendFactor_Zero;
        GraphicsBlendOp colorBlendOp = GraphicsBlendOp_Add;
        GraphicsBlendFactor srcAlphaBlendFactor = GraphicsBlendFactor_Zero;
        GraphicsBlendFactor dstAlphaBlendFactor = GraphicsBlendFactor_Zero;
        GraphicsBlendOp alphaBlendOp = GraphicsBlendOp_Add;
        GraphicsColorComponentFlags colorWriteMask = GraphicsColorComponent_None;
    };

    /// @brief Vertex input state for render pipelines.
    struct GraphicsVertexInputState
    {
        size_t vertexBindingCount = 0;
        GraphicsVertexBinding const* vertexBindings = nullptr;
        size_t vertexAttributeCount= 0;
        GraphicsVertexAttribute const* vertexAttributes = nullptr;
    };

    /// @brief Raster state for render pipelines.
    struct GraphicsRasterState
    {
        GraphicsPolygonMode polygonMode = GraphicsPolygonMode_Fill;
        GraphicsCullMode cullMode = GraphicsCullMode_None;
        bool frontCounterClockwise = false;
        bool depthClampEnable = false;
        bool depthBiasEnable = false;
        float depthBiasConstantFactor = 0.0F;
        float depthBiasSlopeFactor = 0.0F;
        float depthBiasClamp = 0.0F;
    };

    /// @brief Multisample state for render pipelines.
    struct GraphicsMultisampleState
    {
        size_t sampleCount = 1;
        uint32_t sampleMask = 0;
        bool alphaToCoverage = false;
        bool alphaToOne = false;
    };

    /// @brief Depth stencil state for render pipelines.
    struct GraphicsDepthStencilState
    {
        bool depthTest = false;
        bool depthWrite = false;
        GraphicsCompareOp depthCompareOp = GraphicsCompareOp_Never;
        bool stencilTest = false;
        uint32_t stencilReadMask = 0;
        uint32_t stencilWriteMask = 0;
        GraphicsStencilOpState front = {};
        GraphicsStencilOpState back = {};
    };

    /// @brief Blend state for render pipelines.
    struct GraphicsBlendState
    {
        GraphicsLogicOp logicOp = GraphicsLogicOp_Undefined;
        GraphicsRenderTargetBlendState renderTargets[Graphics::MaxRenderTargets];
    };

    /// @brief Render attachment info for render and depth/stencil attachments.
    struct GraphicsRenderAttachmentInfo
    {
        GraphicsTextureView* renderTarget = nullptr;
        GraphicsTextureView* resolveTarget = nullptr;
        GraphicsLoadOp loadOp = GraphicsLoadOp_Load;
        GraphicsStoreOp storeOp = GraphicsStoreOp_Store;
        GraphicsClearValue clearValue = GraphicsClearValue{};
    };

    /// @brief Subresource range for a graphics texture.
    struct GraphicsTextureSubresourceRange
    {
        GraphicsTextureAspectFlags aspectMask = GraphicsTextureAspect_None;
        uint32_t baseMipLevel = 0;
        uint32_t levelCount = Graphics::RemainingMipLevels;
        uint32_t baseArrayLayer = 0;
        uint32_t layerCount = Graphics::RemainingArrayLayers;
    };

    /// @brief Barrier for general pipeline-wide resource states.
    struct GraphicsBarrier
    {
        GraphicsResourceState srcResourceState = GraphicsResourceState_Common;
        GraphicsResourceState dstResourceState = GraphicsResourceState_Common;
    };

    /// @brief Barrier for buffer resource state transitions.
    struct GraphicsBufferBarrier
    {
        GraphicsResourceState srcResourceState = GraphicsResourceState_Common;
        GraphicsResourceState dstResourceState = GraphicsResourceState_Common;
        GraphicsBuffer* buffer = nullptr;
        size_t offset = 0;
        size_t size = 0;
    };

    /// @brief Barrier for texture resource state transitions.
    struct GraphicsTextureBarrier
    {
        GraphicsResourceState srcResourceState = GraphicsResourceState_Common;
        GraphicsResourceState dstResourceState = GraphicsResourceState_Common;
        GraphicsTextureLayout srcLayout = GraphicsTextureLayout_Undefined;
        GraphicsTextureLayout dstLayout = GraphicsTextureLayout_Undefined;
        GraphicsTexture* texture = nullptr;
        GraphicsTextureSubresourceRange subresource = GraphicsTextureSubresourceRange{};
    };

    /// @brief Graphics buffer description.
    struct GraphicsBufferDesc
    {
        GraphicsHeapType heapType = GraphicsHeapType_Default;
        size_t size = 0;
        GraphicsBufferUsageFlags usage = GraphicsBufferUsage_None;
    };

    /// @brief Graphics texture description.
    struct GraphicsTextureDesc
    {
        GraphicsHeapType heapType = GraphicsHeapType_Default;
        GraphicsTextureType textureType = GraphicsTextureType_2D;
        GraphicsFormat textureFormat = GraphicsFormat_Unknown;
        size_t width = 0;
        size_t height = 0;
        size_t depthOrLayers = 1;
        size_t mipLevels = 1;
        size_t sampleCount = 1;
        GraphicsTextureTiling tilingMode = GraphicsTextureTiling_Optimal;
        GraphicsTextureUsageFlags usage = GraphicsTextureUsage_None;
    };

    /// @brief Graphics texture view description.
    struct GraphicsTextureViewDesc
    {
        GraphicsTexture* texture = nullptr;
        GraphicsTextureViewType viewType = GraphicsTextureViewType_1D;
        GraphicsFormat viewFormat = GraphicsFormat_Unknown;
        GraphicsTextureSubresourceRange subresourceRange = {};
    };

    /// @brief Graphics sampler description.
    struct GraphicsSamplerDesc
    {
        GraphicsFilter magFilter = GraphicsFilter_Nearest;
        GraphicsFilter minFilter = GraphicsFilter_Nearest;
        GraphicsSamplerMipmapMode mipmapMode = GraphicsSamplerMipmapMode_Nearest;
        GraphicsSamplerAddressMode addressModeU = GraphicsSamplerAddressMode_Repeat;
        GraphicsSamplerAddressMode addressModeV = GraphicsSamplerAddressMode_Repeat;
        GraphicsSamplerAddressMode addressModeW = GraphicsSamplerAddressMode_Repeat;
        GraphicsCompareOp compareOp = GraphicsCompareOp_Undefined;
        uint32_t maxAnisotropy = 0;
        float mipLodBias = 0.0F;
        float minLod = 0.0F;
        float maxLod = Graphics::LODClampNone;
        GraphicsBorderColor borderColor = GraphicsBorderColor_TransparentBlack_Float;
    };

    /// @brief Graphics descriptor set layout description.
    struct GraphicsDescriptorSetLayoutDesc
    {
        size_t bindingCount = 0;
        GraphicsDescriptorSetLayoutBinding const* bindings = nullptr;
    };

    /// @brief Graphics descriptor set description.
    struct GraphicsDescriptorSetDesc
    {
        GraphicsDescriptorSetLayout* descriptorSetLayout = nullptr;
        uint32_t binding = 0;
        uint32_t arrayElement = 0;
        uint32_t descriptorCount = 0;
        GraphicsDescriptorType descriptorType = GraphicsDescriptorType_Sampler;
        GraphicsDescriptorBufferInfo const* bufferInfos = nullptr;
        GraphicsDescriptorImageInfo const* imageInfos = nullptr;
    };

    /// @brief Graphics pipeline layout description.
    struct GraphicsPipelineLayoutDesc
    {
        size_t descriptorSetLayoutCount = 0;
        GraphicsDescriptorSetLayout* const* descriptorSetLayouts = nullptr;
        size_t pushConstantRangeCount = 0;
        GraphicsPushConstantRange const* pushConstantRanges = nullptr;
    };

    /// @brief Graphics render pipeline description.
    struct GraphicsRenderPipelineDesc
    {
        GraphicsPipelineLayout* pipelineLayout = nullptr;

        GraphicsShaderEntrypoint vertexShaderEntrypoint;
        GraphicsShaderEntrypoint tessControlShaderEntrypoint;
        GraphicsShaderEntrypoint tessEvalShaderEntrypoint;
        GraphicsShaderEntrypoint fragmentShaderEntrypoint;
        
        GraphicsShaderBytecode vertexShaderBytecode;
        GraphicsShaderBytecode tessControlShaderBytecode;
        GraphicsShaderBytecode tessEvalShaderBytecode;
        GraphicsShaderBytecode fragmentShaderBytecode;

        GraphicsVertexInputState vertexInputState;
        GraphicsPrimitiveTopology primitiveTopology = GraphicsPrimitiveTopology_PointList;
        GraphicsIndexBufferStripCutValue stripCutValue = GraphicsIndexBufferStripCutValue_None;
        GraphicsRasterState rasterState;
        GraphicsMultisampleState multisampleState;
        GraphicsDepthStencilState depthStencilState;
        GraphicsBlendState blendState;

        size_t renderTargetCount = 0;
        GraphicsFormat renderTargetFormats[Graphics::MaxRenderTargets];
        GraphicsFormat depthStencilFormat = GraphicsFormat_Unknown;
    };

    /// @brief Graphics compute pipeline description.
    struct GraphicsComputePipelineDesc
    {
        GraphicsPipelineLayout* pipelineLayout;
        GraphicsShaderEntrypoint computeShaderEntrypoint;
        GraphicsShaderBytecode computeShaderBytecode;
    };

    /// @brief Graphics swapchain description.
    struct GraphicsSwapchainDesc
    {
        SDL_Window* window;
        GraphicsFormat preferredformat;
        GraphicsFormat fallbackFormat;
        uint32_t width;
        uint32_t height;
        uint32_t syncInterval;
        bool allowTearing;
    };

    /// @brief Test if a bit is set in a bitflag type.
    /// @param flags Flags to test.
    /// @param bit Bit to test.
    /// @return `true` if the bitflag was set.
    template<typename FlagsType, typename BitType>
    static constexpr bool IsBitSet(FlagsType const& flags, BitType const& bit)
    {
        return (flags & bit) == bit;
    }
} // namespace Bonsai

/// @brief Convert a cstring to a GraphicsShaderEntrypoint.
/// @param name Target cstring.
/// @param size Target cstring length.
/// @return The graphics shader entrypoint.
static constexpr Bonsai::GraphicsShaderEntrypoint operator""_entrypoint(char const* name, size_t size)
{
    return Bonsai::GraphicsShaderEntrypoint{ name, size };
}

#endif //BONSAI_GRAPHICS_TYPES_HPP
