#pragma once
#ifndef BONSAI_GRAPHICS_COMMAND_BUFFER_INTERNAL_HPP
#define BONSAI_GRAPHICS_COMMAND_BUFFER_INTERNAL_HPP

#include <cstdint>
#include <vector>
#include "Gfx/GraphicsCommandBuffer.hpp"
#include "Gfx/Internal/GraphicsCommands.hpp"

namespace Bonsai
{
    /// @brief Graphics command buffer implementation.
    class GraphicsCommandBufferImpl : public GraphicsCommandBuffer
    {
    public:
        GraphicsCommandBufferImpl();

        GraphicsCommandBufferImpl(GraphicsCommandBufferImpl const&) = delete;
        GraphicsCommandBufferImpl& operator=(GraphicsCommandBufferImpl const&) = delete;
        
        GraphicsCommandBufferImpl(GraphicsCommandBufferImpl&&) = default;
        GraphicsCommandBufferImpl& operator=(GraphicsCommandBufferImpl&&) = default;
        
        /// @brief Reset the command buffer.
        void Reset() override final;

        void PipelineBarrier(
            GraphicsBarrier* globalBarriers,
            size_t globalBarrierCount,
            GraphicsBufferBarrier* bufferBarriers,
            size_t bufferBarrierCount,
            GraphicsTextureBarrier* textureBarriers,
            size_t textureBarrierCount
        ) override final;

        void CopyBufferToBuffer(
            GraphicsBuffer* src,
            GraphicsBuffer* dst,
            size_t size,
            size_t srcOffset,
            size_t dstOffset
        ) override final;

        void CopyBufferToTexture(
            GraphicsBuffer* src,
            GraphicsTexture* dst,
            size_t bufferOffset,
            size_t bufferRowLength,
            size_t bufferImageHeight,
            GraphicsTextureSubresourceRange const& textureSubresource,
            Offset3D const& textureOffset,
            Extent3D const& textureExtent
        ) override final;

        void CopyTextureToBuffer(
            GraphicsTexture* src,
            GraphicsBuffer* dst,
            size_t bufferOffset,
            size_t bufferImagePitch,
            size_t bufferImageHeight,
            GraphicsTextureSubresourceRange const& textureSubresource,
            Offset3D const& textureOffset,
            Extent3D const& textureExtent
        ) override final;

        void CopyTextureToTexture(
            GraphicsTexture* src,
            GraphicsTexture* dst,
            Offset3D const& srcOffset,
            GraphicsTextureSubresourceRange const& srcSubresource,
            Offset3D const& dstOffset,
            GraphicsTextureSubresourceRange const& dstSubresource,
            Extent3D const& extent
        ) override final;

        void ClearRenderTarget(
            GraphicsTextureView* textureView,
            GraphicsClearColorValue const& clearValue
        ) override final;

        void ClearDepthStencilTarget(
            GraphicsTextureView* textureView,
            GraphicsClearDepthStencilValue const& clearValue
        ) override final;

        void SetViewport(
            GraphicsViewport const& viewport
        ) override final;

        void SetScissorRect(
            Rect2D const& scissorRect
        ) override final;

        void SetLineWidth(
            float lineWidth
        ) override final;

        void SetBlendConstants(
            float r,
            float g,
            float b,
            float a
        ) override final;

        void SetStencilReference(
            uint32_t stencilReference
        ) override final;

        void BindRenderPipeline(
            GraphicsRenderPipeline* renderPipeline
        ) override final;

        void BindComputePipeline(
            GraphicsComputePipeline* computePipeline
        ) override final;

        void PushConstants(
            GraphicsPipelineLayout* pipelineLayout,
            GraphicsShaderStageFlags stages,
            size_t offset,
            size_t size,
            void const* data
        ) override final;

        void BindRenderDescriptorSets(
            GraphicsPipelineLayout* pipelineLayout,
            size_t firstSet,
            GraphicsDescriptorSet* const* descriptorSets,
            size_t descriptorSetCount
        ) override final;

        void BindComputeDescriptorSets(
            GraphicsPipelineLayout* pipelineLayout,
            size_t firstSet,
            GraphicsDescriptorSet* const* descriptorSets,
            size_t descriptorSetCount
        ) override final;

        void BindVertexBuffers(
            size_t firstSlot,
            GraphicsBuffer* const* buffers,
            size_t bufferCount,
            size_t* offsets
        ) override final;

        void BindIndexBuffer(
            GraphicsBuffer* buffer,
            size_t offset,
            GraphicsIndexType indexType
        ) override final;

        void DrawIndexed(
            uint32_t indexCount,
            uint32_t instanceCount,
            uint32_t firstIndex,
            int32_t vertexOffset,
            uint32_t firstInstance
        ) override final;

        void DrawIndexedIndirect(
            GraphicsBuffer* buffer,
            size_t offset,
            uint32_t drawCount,
            uint32_t stride
        ) override final;

        void Dispatch(
            uint32_t sizeX,
            uint32_t sizeY,
            uint32_t sizeZ
        ) override final;

        void DispatchIndirect(
            GraphicsBuffer* buffer,
            size_t offset
        ) override final;

        void BeginRendering(
            Rect2D const& renderArea,
            GraphicsRenderAttachmentInfo* renderAttachments,
            size_t renderAttachmentCount,
            GraphicsRenderAttachmentInfo* depthStencilAttachment
        ) override final;

        void EndRendering() override final;

        /// @brief Check if the command buffer is ready for recording new commands.
        /// @return The command buffer ready state.
        bool IsReady() const { return _commands.empty(); }

        /// @brief Get the recorded commands for the command buffer.
        /// @return The list of recorded commands.
        std::vector<GraphicsCommand> const& GetRecordedCommands() const { return _commands; }

    private:
        /// @brief Expected number of commands recorded per command buffer.
        static constexpr uint32_t ExpectedCommandsPerCommandBuffer = 512;

        std::vector<GraphicsCommand> _commands; //< Commands recorded in command buffer
    };
} // namespace Bonsai

#endif //BONSAI_GRAPHICS_COMMAND_BUFFER_INTERNAL_HPP
