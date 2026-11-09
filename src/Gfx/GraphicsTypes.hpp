#pragma once
#ifndef BONSAI_GRAPHICS_TYPES_HPP
#define BONSAI_GRAPHICS_TYPES_HPP

#include <cstddef>
#include <cstdint>
#include <SDL3/SDL.h>

namespace Bonsai
{
    class GraphicsBuffer;
    class GraphicsSwapchain;
    class FrameContext;

    /// @brief Enumeration of available graphics heap types.
    enum GraphicsHeapType : uint32_t
    {
        GraphicsHeapType_Default    = 0,
        GraphicsHeapType_Upload     = 1,
        GraphicsHeapType_ReadBack   = 2,
    };

    /// @brief Enumeration of graphics buffer usage flag bit.
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

    /// @brief Graphics buffer description.
    struct GraphicsBufferDesc
    {
        GraphicsHeapType heapType = GraphicsHeapType_Default;
        size_t size = 0;
        GraphicsBufferUsageFlags usage = GraphicsBufferUsage_None;
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
