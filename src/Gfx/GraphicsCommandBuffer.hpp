#pragma once
#ifndef BONSAI_GRAPHICS_COMMAND_BUFFER_HPP
#define BONSAI_GRAPHICS_COMMAND_BUFFER_HPP

#include <cstdint>
#include <vector>
#include "Gfx/GraphicsCommands.hpp"
#include "Gfx/GraphicsTypes.hpp"

namespace Bonsai
{
    /// @brief The GraphicsCommandBuffer is used to record graphics commands
    /// that can then be submitted to a GraphicsDevice.
    class GraphicsCommandBuffer
    {
    public:
        GraphicsCommandBuffer();
        
        /// @brief Reset the command buffer.
        void Reset();

        void CopyBufferToBuffer(
            GraphicsBuffer* src,
            GraphicsBuffer* dst,
            size_t size,
            size_t srcOffset,
            size_t dstOffset
        );

        void CopyBufferToTexture(
            GraphicsBuffer* src,
            GraphicsTexture* dst,
            size_t bufferOffset,
            size_t bufferRowLength,
            size_t bufferImageHeight,
            GraphicsTextureSubresourceRange const& textureSubresource,
            Offset3D const& textureOffset,
            Extent3D const& textureExtent
        );

        void CopyTextureToBuffer(
            GraphicsTexture* src,
            GraphicsBuffer* dst,
            size_t bufferOffset,
            size_t bufferImagePitch,
            size_t bufferImageHeight,
            GraphicsTextureSubresourceRange const& textureSubresource,
            Offset3D const& textureOffset,
            Extent3D const& textureExtent
        );

        void CopyTextureToTexture(
            GraphicsTexture* src,
            GraphicsTexture* dst,
            Offset3D const& srcOffset,
            GraphicsTextureSubresourceRange const& srcSubresource,
            Offset3D const& dstOffset,
            GraphicsTextureSubresourceRange const& dstSubresource,
            Extent3D const& extent
        );

        void ClearRenderTarget(
            GraphicsTextureView* textureView,
            GraphicsClearColorValue const& clearValue
        );

        void ClearDepthStencilTarget(
            GraphicsTextureView* textureView,
            GraphicsClearDepthStencilValue const& clearValue
        );

        void BindVertexBuffers(
            size_t firstSlot,
            GraphicsBuffer** buffers,
            size_t bufferCount,
            size_t* offsets
        );

        void BindIndexBuffer(
            GraphicsBuffer* buffer,
            size_t offset,
            GraphicsIndexType indexType
        );

        void DrawIndexed(
            uint32_t indexCount,
            uint32_t instanceCount,
            uint32_t firstIndex,
            int32_t vertexOffset,
            uint32_t firstInstance
        );

        void DrawIndexedIndirect(
            GraphicsBuffer* buffer,
            size_t offset,
            uint32_t drawCount,
            uint32_t stride
        );

        void Dispatch(
            uint32_t sizeX,
            uint32_t sizeY,
            uint32_t sizeZ
        );

        void DispatchIndirect(
            GraphicsBuffer* buffer,
            size_t offset
        );

        void BeginRendering(
            GraphicsRenderAttachmentInfo* renderAttachments,
            size_t renderAttachmentCount,
            GraphicsRenderAttachmentInfo* depthStencilAttachment
        );

        void EndRendering();

        /// @brief Get the recorded commands for the command buffer.
        /// @return The list of recorded commands.
        std::vector<GraphicsCommand> const& GetRecordedCommands() const { return _commands; }

    private:
        /// @brief Expected number of commands recorded per command buffer.
        static constexpr uint32_t ExpectedCommandsPerCommandBuffer = 512;

        std::vector<GraphicsCommand> _commands; //< Commands recorded in command buffer
    };
} // namespace Bonsai

#endif //BONSAI_GRAPHICS_COMMAND_BUFFER_HPP
