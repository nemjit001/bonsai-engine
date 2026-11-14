#pragma once
#ifndef BONSAI_GRAPHICS_COMMAND_BUFFER_HPP
#define BONSAI_GRAPHICS_COMMAND_BUFFER_HPP

#include "Gfx/GraphicsTypes.hpp"

namespace Bonsai
{
    /// @brief The GraphicsCommandBuffer is used to record graphics commands
    /// that can then be submitted to a GraphicsDevice.
    class GraphicsCommandBuffer
    {
    public:
        virtual ~GraphicsCommandBuffer() = default;

        /// @brief Reset the command buffer.
        virtual void Reset() = 0;

        virtual void CopyBufferToBuffer(
            GraphicsBuffer* src,
            GraphicsBuffer* dst,
            size_t size,
            size_t srcOffset,
            size_t dstOffset
        ) = 0;

        virtual void CopyBufferToTexture(
            GraphicsBuffer* src,
            GraphicsTexture* dst,
            size_t bufferOffset,
            size_t bufferRowLength,
            size_t bufferImageHeight,
            GraphicsTextureSubresourceRange const& textureSubresource,
            Offset3D const& textureOffset,
            Extent3D const& textureExtent
        ) = 0;

        virtual void CopyTextureToBuffer(
            GraphicsTexture* src,
            GraphicsBuffer* dst,
            size_t bufferOffset,
            size_t bufferImagePitch,
            size_t bufferImageHeight,
            GraphicsTextureSubresourceRange const& textureSubresource,
            Offset3D const& textureOffset,
            Extent3D const& textureExtent
        ) = 0;

        virtual void CopyTextureToTexture(
            GraphicsTexture* src,
            GraphicsTexture* dst,
            Offset3D const& srcOffset,
            GraphicsTextureSubresourceRange const& srcSubresource,
            Offset3D const& dstOffset,
            GraphicsTextureSubresourceRange const& dstSubresource,
            Extent3D const& extent
        ) = 0;

        virtual void ClearRenderTarget(
            GraphicsTextureView* textureView,
            GraphicsClearColorValue const& clearValue
        ) = 0;

        virtual void ClearDepthStencilTarget(
            GraphicsTextureView* textureView,
            GraphicsClearDepthStencilValue const& clearValue
        ) = 0;

        virtual void BindVertexBuffers(
            size_t firstSlot,
            GraphicsBuffer** buffers,
            size_t bufferCount,
            size_t* offsets
        ) = 0;

        virtual void BindIndexBuffer(
            GraphicsBuffer* buffer,
            size_t offset,
            GraphicsIndexType indexType
        ) = 0;

        virtual void DrawIndexed(
            uint32_t indexCount,
            uint32_t instanceCount,
            uint32_t firstIndex,
            int32_t vertexOffset,
            uint32_t firstInstance
        ) = 0;

        virtual void DrawIndexedIndirect(
            GraphicsBuffer* buffer,
            size_t offset,
            uint32_t drawCount,
            uint32_t stride
        ) = 0;

        virtual void Dispatch(
            uint32_t sizeX,
            uint32_t sizeY,
            uint32_t sizeZ
        ) = 0;

        virtual void DispatchIndirect(
            GraphicsBuffer* buffer,
            size_t offset
        ) = 0;

        virtual void BeginRendering(
            GraphicsRenderAttachmentInfo* renderAttachments,
            size_t renderAttachmentCount,
            GraphicsRenderAttachmentInfo* depthStencilAttachment
        ) = 0;

        virtual void EndRendering() = 0;
    };
} // namespace Bonsai

#endif //BONSAI_GRAPHICS_COMMAND_BUFFER_HPP
