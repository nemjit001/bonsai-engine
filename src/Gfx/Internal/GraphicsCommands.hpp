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

        GraphicsCommandType_PipelineBarrier,

        GraphicsCommandType_CopyBufferToBuffer,
        GraphicsCommandType_CopyBufferToTexture,
        GraphicsCommandType_CopyTextureToBuffer,
        GraphicsCommandType_CopyTextureToTexture,

        GraphicsCommandType_ClearRenderTarget,
        GraphicsCommandType_ClearDepthStencilTarget,

        GraphicsCommandType_SetViewport,
        GraphicsCommandType_SetScissorRect,
        GraphicsCommandType_SetLineWidth,
        GraphicsCommandType_SetBlendConstants,
        GraphicsCommandType_SetStencilReference,

        GraphicsCommandType_BindRenderPipeline,
        GraphicsCommandType_BindComputePipeline,
        GraphicsCommandType_PushConstants,
        GraphicsCommandType_BindRenderDescriptorSets,
        GraphicsCommandType_BindComputeDescriptorSets,
        GraphicsCommandType_BindVertexBuffers,
        GraphicsCommandType_BindIndexBuffer,

        GraphicsCommandType_Draw,
        GraphicsCommandType_DrawIndexed,
        GraphicsCommandType_DrawIndexedIndirect,

        GraphicsCommandType_Dispatch,
        GraphicsCommandType_DispatchIndirect,

        GraphicsCommandType_BeginRendering,
        GraphicsCommandType_EndRendering,
    };

    struct GraphicsCommandPipelineBarrier
    {
        size_t globalBarrierCount;
        size_t bufferBarrierCount;
        size_t textureBarrierCount;
        std::array<GraphicsBarrier, Graphics::MaxGlobalBarriers> globalBarriers;
        std::array<GraphicsBufferBarrier, Graphics::MaxBufferBarriers> bufferBarriers;
        std::array<GraphicsTextureBarrier, Graphics::MaxTextureBarriers> textureBarriers;
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
        size_t bufferRowLength;
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
        size_t bufferRowLength;
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

    struct GraphicsCommandSetViewport
    {
        GraphicsViewport viewport;
    };

    struct GraphicsCommandSetScissorRect
    {
        Rect2D scissorRect;
    };

    struct GraphicsCommandSetLineWidth
    {
        float lineWidth;
    };

    struct GraphicsCommandSetBlendConstants
    {
        float blendConstants[4];
    };

    struct GraphicsCommandSetStencilReference
    {
        uint32_t stencilReference;
    };

    struct GraphicsCommandBindRenderPipeline
    {
        GraphicsRenderPipeline* renderPipeline;
    };

    struct GraphicsCommandBindComputePipeline
    {
        GraphicsComputePipeline* computePipeline;
    };

    struct GraphicsCommandPushConstants
    {
        GraphicsPipelineLayout* pipelineLayout;
        GraphicsShaderStageFlags stages;
        size_t offset;
        size_t size;
        std::array<uint8_t, Graphics::MaxPushConstantsSize> data;
    };

    struct GraphicsCommandBindDescriptorSets
    {
        GraphicsPipelineLayout* pipelineLayout;
        size_t firstSet;
        size_t descriptorSetCount;
        std::array<GraphicsDescriptorSet*, Graphics::MaxBoundDescriptorSets> descriptorSets;
    };

    struct GraphicsCommandBindVertexBuffers
    {
        size_t firsSlot;
        size_t bufferCount;
        std::array<GraphicsBuffer*, Graphics::MaxVertexInputBindings> buffers;
        std::array<size_t, Graphics::MaxVertexInputBindings> offsets;
    };

    struct GraphicsCommandBindIndexBuffer
    {
        GraphicsBuffer* buffer;
        size_t offset;
        GraphicsIndexType indexType;
    };

    struct GraphicsCommandDraw
    {
        uint32_t vertexCount;
        uint32_t instanceCount;
        uint32_t firstVertex;
        uint32_t firstInstance;
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
        Rect2D renderArea;
        size_t renderAttachmentCount;
        bool hasDepthStencilAttachment;
        std::array<GraphicsRenderAttachmentInfo, Graphics::MaxRenderTargets> renderAttachments;
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
            GraphicsCommandPipelineBarrier commandPipelineBarrier;
            GraphicsCommandCopyBufferToBuffer commandCopyBufferToBuffer;
            GraphicsCommandCopyBufferToTexture commandCopyBufferToTexture;
            GraphicsCommandCopyTextureToBuffer commandCopyTextureToBuffer;
            GraphicsCommandCopyTextureToTexture commandCopyTextureToTexture;
            GraphicsCommandClearRenderTarget commandClearRenderTarget;
            GraphicsCommandClearDepthStencilTarget commandClearDepthStencilTarget;
            GraphicsCommandSetViewport commandSetViewport;
            GraphicsCommandSetScissorRect commandSetScissorRect;
            GraphicsCommandSetLineWidth commandSetLineWidth;
            GraphicsCommandSetBlendConstants commandSetBlendConstants;
            GraphicsCommandSetStencilReference commandSetStencilReference;
            GraphicsCommandBindRenderPipeline commandBindRenderPipeline;
            GraphicsCommandBindComputePipeline commandBindComputePipeline;
            GraphicsCommandPushConstants commandPushConstants;
            GraphicsCommandBindDescriptorSets commandBindDescriptorSets;
            GraphicsCommandBindVertexBuffers commandBindVertexBuffers;
            GraphicsCommandBindIndexBuffer commandBindIndexBuffer;
            GraphicsCommandDraw commandDraw;
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
