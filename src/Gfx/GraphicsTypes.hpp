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
    class GraphicsSwapchain;
    class FrameContext;

    /// @brief Enumeration of available graphics heap types.
    enum GraphicsHeapType : uint32_t
    {
        GraphicsHeapType_Default    = 0,
        GraphicsHeapType_Upload     = 1,
        GraphicsHeapType_ReadBack   = 2,
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

        GraphicsFormat_RG32_UNorm,
        GraphicsFormat_RG32_SNorm,
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

    /// @brief Enumeration of available graphics texture tiling modes.
    enum GraphicsTextureTiling : uint32_t
    {
        GraphicsTextureTiling_Linear    = 0,
        GraphicsTextureTiling_Optimal   = 1,
    };

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

    /// @brief Graphics buffer description.
    struct GraphicsBufferDesc
    {
        GraphicsHeapType heapType = GraphicsHeapType_Default;
        size_t size = 0;
        GraphicsBufferUsageFlags usage = GraphicsBufferUsage_None;
    };

    struct GraphicsTextureDesc
    {
        GraphicsHeapType heapType = GraphicsHeapType_Default;
        GraphicsTextureType textureType = GraphicsTextureType_2D;
        GraphicsFormat textureFormat = GraphicsFormat_Unknown;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t depthOrLayers = 1;
        uint32_t mipLevels = 1;
        uint32_t sampleCount = 1;
        GraphicsTextureTiling tilingMode = GraphicsTextureTiling_Optimal;
        GraphicsTextureUsageFlags usage = GraphicsTextureUsage_None;
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
