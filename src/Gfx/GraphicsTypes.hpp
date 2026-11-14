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
    class GraphicsSwapchain;
    class GraphicsCommandBuffer;
    class FrameContext;
    class TransferBatch;

    namespace Graphics
    {
        static constexpr uint32_t   MaxBoundRenderTargets       = 8u;
        static constexpr uint32_t   MaxBoundVertexBuffers       = 16u;
        static constexpr uint32_t   MaxGraphicsCommandBuffers   = 32u;

        static constexpr uint32_t   RemainingMipLevels      = UINT32_MAX;
        static constexpr uint32_t   RemainingArrayLayers    = UINT32_MAX;
        static constexpr float      LODClampNone            = 1'000.0F;
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

    struct GraphicsRenderAttachmentInfo
    {
        GraphicsTextureView* renderTarget;
        GraphicsTextureView* resolveTarget;
        GraphicsLoadOp loadOp;
        GraphicsStoreOp storeOp;
        GraphicsClearValue clearValue;
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

    /// @brief Graphics swapchain description.
    struct GraphicsSwapchainDesc
    {
        SDL_Window* window;
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

#endif //BONSAI_GRAPHICS_TYPES_HPP
