#pragma once
#ifndef BONSAI_GRAPHICS_COMMAND_BUFFER_HPP
#define BONSAI_GRAPHICS_COMMAND_BUFFER_HPP

#include <cstdint>
#include <Gfx/GraphicsTypes.hpp>

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

    /// @brief A GraphicsCommand represents a single command within a graphics command buffer.
    class GraphicsCommand
    {
    public:
        virtual ~GraphicsCommand() = default;

        GraphicsCommand(GraphicsCommand const&) = default;
        GraphicsCommand& operator=(GraphicsCommand const&) = default;

        /// @brief Get the graphics command type of this command.
        /// @return The graphics command type.
        virtual GraphicsCommandType GetCommandType() const final { return _type; }

    protected:
        GraphicsCommandType _type = GraphicsCommandType_Undefined;
    };

    /// @brief The GraphicsCommandBuffer is used to record graphics commands
    /// that can then be submitted to a GraphicsDevice.
    class GraphicsCommandBuffer
    {
    public:
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
            size_t bufferImagePitch,
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
            GraphicsTexture const& dstSubresource,
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
    };
} // namespace Bonsai

#endif //BONSAI_GRAPHICS_COMMAND_BUFFER_HPP
