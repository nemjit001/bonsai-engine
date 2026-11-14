#pragma once
#ifndef BONSAI_GRAPHICS_COMMAND_HPP
#define BONSAI_GRAPHICS_COMMAND_HPP

#include <array>
#include "Gfx/GraphicsTypes.hpp"

namespace Bonsai
{
    /// @brief Enumeration of graphics command types.
    enum GraphicsCommandType : uint32_t
    {
        GraphicsCommandType_Undefined               = 0,

        GraphicsCommandType_CopyBufferToBuffer      = 1,
        GraphicsCommandType_CopyBufferToTexture     = 2,
        GraphicsCommandType_CopyTextureToBuffer     = 3,
        GraphicsCommandType_CopyTextureToTexture    = 4,

        GraphicsCommandType_ClearRenderTarget       = 5,
        GraphicsCommandType_ClearDepthStencilTarget = 6,

        GraphicsCommandType_BindVertexBuffers       = 7,
        GraphicsCommandType_BindIndexBuffer         = 8,
        GraphicsCommandType_DrawIndexed             = 9,
        GraphicsCommandType_DrawIndexedIndirect     = 10,

        GraphicsCommandType_Dispatch                = 11,
        GraphicsCommandType_DispatchIndirect        = 12,

        GraphicsCommandType_BeginRendering          = 13,
        GraphicsCommandType_EndRendering            = 14,
    };

    struct GraphicsCommandCopyBufferToBuffer
    {
        GraphicsBuffer* src;
        GraphicsBuffer* dst;
        size_t size;
        size_t srcOffset;
        size_t dstOffset;
    };

    struct GraphicsCommandCopyBufferToTexture
    {
        GraphicsBuffer* src;
        GraphicsTexture* dst;
        size_t bufferOffset;
        size_t bufferImagePitch;
        size_t bufferImageHeight;
        GraphicsTextureSubresourceRange textureSubresource;
        Offset3D textureOffset;
        Extent3D textureExtent;
    };

    struct GraphicsCommandCopyTextureToBuffer
    {
        GraphicsTexture* src;
        GraphicsBuffer* dst;
        size_t bufferOffset;
        size_t bufferImagePitch;
        size_t bufferImageHeight;
        GraphicsTextureSubresourceRange textureSubresource;
        Offset3D textureOffset;
        Extent3D textureExtent;
    };

    struct GraphicsCommandCopyTextureToTexture
    {
        GraphicsTexture* src;
        GraphicsTexture* dst;
        Offset3D srcOffset;
        GraphicsTextureSubresourceRange srcSubresource;
        Offset3D dstOffset;
        GraphicsTextureSubresourceRange dstSubresource;
        Extent3D extent;
    };

    struct GraphicsCommandClearRenderTarget
    {
        GraphicsTextureView* textureView;
        GraphicsClearColorValue clearValue;
    };

    struct GraphicsCommandClearDepthStencilTarget
    {
        GraphicsTextureView* textureView;
        GraphicsClearDepthStencilValue clearValue;
    };

    struct GraphicsCommandBindVertexBuffers
    {
        size_t firsSlot;
        std::array<GraphicsBuffer*, Graphics::MaxBoundVertexBuffers> buffers;
        size_t bufferCount;
        std::array<size_t, Graphics::MaxBoundVertexBuffers> offsets;
    };

    struct GraphicsCommandBindIndexBuffer
    {
        GraphicsBuffer* buffer;
        size_t offset;
        GraphicsIndexType indexType;
    };

    struct GraphicsCommandDrawIndexed
    {
        uint32_t indexCount;
        uint32_t instanceCount;
        uint32_t firstIndex;
        int32_t vertexOffset;
        uint32_t firstInstance;
    };

    struct GraphicsCommandDrawIndexedIndirect
    {
        GraphicsBuffer* buffer;
        size_t offset;
        uint32_t drawCount;
        uint32_t stride;
    };

    struct GraphicsCommandDispatch
    {
        uint32_t sizeX;
        uint32_t sizeY;
        uint32_t sizeZ;
    };

    struct GraphicsCommandDispatchIndirect
    {
        GraphicsBuffer* buffer;
        size_t offset;
    };

    struct GraphicsCommandBeginRendering
    {
        std::array<GraphicsRenderAttachmentInfo, Graphics::MaxBoundRenderTargets> renderAttachments;
        size_t renderAttachmentCount;
        bool hasDepthStencilAttachment;
        GraphicsRenderAttachmentInfo depthStencilAttachment;
    };

    struct GraphicsCommandEndRendering
    {
        // Empty :)
    };

    /// @brief A GraphicsCommand represents a single command within a graphics command buffer.
    /// It is a tagged union of graphics command structures.
    struct GraphicsCommand
    {
        GraphicsCommandType type = GraphicsCommandType_Undefined;
        union GraphicsCommandData
        {
            GraphicsCommandCopyBufferToBuffer commandCopyBufferToBuffer;
            GraphicsCommandCopyBufferToTexture commandCopyBufferToTexture;
            GraphicsCommandCopyTextureToBuffer commandCopyTextureToBuffer;
            GraphicsCommandCopyTextureToTexture commandCopyTextureToTexture;
            GraphicsCommandClearRenderTarget commandClearRenderTarget;
            GraphicsCommandClearDepthStencilTarget commandClearDepthStencilTarget;
            GraphicsCommandBindVertexBuffers commandBindVertexBuffers;
            GraphicsCommandBindIndexBuffer commandBindIndexBuffer;
            GraphicsCommandDrawIndexed commandDrawIndexed;
            GraphicsCommandDrawIndexedIndirect commandDrawIndexedIndirect;
            GraphicsCommandDispatch commandDispatch;
            GraphicsCommandDispatchIndirect commandDispatchIndirect;
            GraphicsCommandBeginRendering commandBeginRendering;
            GraphicsCommandEndRendering commandEndRendering;
        } data;
    };
} // namespace Bonsai

#endif //BONSAI_GRAPHICS_COMMAND_HPP
