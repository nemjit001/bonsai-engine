#include "GraphicsCommandBufferInternal.hpp"

#include <algorithm>

namespace Bonsai
{
    GraphicsCommandBufferImpl::GraphicsCommandBufferImpl()
    {
        // Ensure to reserve at least enough space for the expected number of commands
        _commands.reserve(ExpectedCommandsPerCommandBuffer);
    }

    void GraphicsCommandBufferImpl::Reset()
    {
        _commands.clear();
    }

    void GraphicsCommandBufferImpl::PipelineBarrier(
        GraphicsBarrier* globalBarriers,
        size_t globalBarrierCount,
        GraphicsBufferBarrier* bufferBarriers,
        size_t bufferBarrierCount,
        GraphicsTextureBarrier* textureBarriers,
        size_t textureBarrierCount
    )
    {
        // Ensure barrier counts don't exceed maximum
        globalBarrierCount = std::min(globalBarrierCount, static_cast<size_t>(Graphics::MaxGlobalBarriers));
        bufferBarrierCount = std::min(bufferBarrierCount, static_cast<size_t>(Graphics::MaxBufferBarriers));
        textureBarrierCount = std::min(textureBarrierCount, static_cast<size_t>(Graphics::MaxTextureBarriers));

        // Copy over barrier data
        std::array<GraphicsBarrier, Graphics::MaxGlobalBarriers> storedGlobalBarriers;
        for (size_t i = 0; i < globalBarrierCount; i++) {
            storedGlobalBarriers[i] = globalBarriers[i];
        }

        std::array<GraphicsBufferBarrier, Graphics::MaxBufferBarriers> storedBufferBarriers;
        for (size_t i = 0; i < bufferBarrierCount; i++) {
            storedBufferBarriers[i] = bufferBarriers[i];
        }

        std::array<GraphicsTextureBarrier, Graphics::MaxTextureBarriers> storedTextureBarriers;
        for (size_t i = 0; i < textureBarrierCount; i++) {
            storedTextureBarriers[i] = textureBarriers[i];
        }

        // Set up command state
        GraphicsCommandPipelineBarrier commandPipelineBarrier{};
        commandPipelineBarrier.globalBarriers = storedGlobalBarriers;
        commandPipelineBarrier.globalBarrierCount = globalBarrierCount;
        commandPipelineBarrier.bufferBarriers = storedBufferBarriers;
        commandPipelineBarrier.bufferBarrierCount = bufferBarrierCount;
        commandPipelineBarrier.textureBarriers = storedTextureBarriers;        
        commandPipelineBarrier.textureBarrierCount = textureBarrierCount;

        GraphicsCommand command{};
        command.type = GraphicsCommandType_PipelineBarrier;
        command.data.commandPipelineBarrier = commandPipelineBarrier;

        _commands.push_back(command);
    }

    void GraphicsCommandBufferImpl::CopyBufferToBuffer(
        GraphicsBuffer* src,
        GraphicsBuffer* dst,
        size_t size,
        size_t srcOffset,
        size_t dstOffset
    )
    {
        GraphicsCommandCopyBufferToBuffer commandCopyBufferToBuffer{};
        commandCopyBufferToBuffer.src = src;
        commandCopyBufferToBuffer.dst = dst;
        commandCopyBufferToBuffer.size = size;
        commandCopyBufferToBuffer.srcOffset = srcOffset;
        commandCopyBufferToBuffer.dstOffset = dstOffset;

        GraphicsCommand command{};
        command.type = GraphicsCommandType_CopyBufferToBuffer;
        command.data.commandCopyBufferToBuffer = commandCopyBufferToBuffer;

        _commands.push_back(command);
    }

    void GraphicsCommandBufferImpl::CopyBufferToTexture(
        GraphicsBuffer* src,
        GraphicsTexture* dst,
        size_t bufferOffset,
        size_t bufferRowLength,
        size_t bufferImageHeight,
        GraphicsTextureSubresourceRange const& textureSubresource,
        Offset3D const& textureOffset,
        Extent3D const& textureExtent
    )
    {
        GraphicsCommandCopyBufferToTexture commandCopyBufferToTexture{};
        commandCopyBufferToTexture.src = src;
        commandCopyBufferToTexture.dst = dst;
        commandCopyBufferToTexture.bufferOffset = bufferOffset;
        commandCopyBufferToTexture.bufferRowLength = bufferRowLength;
        commandCopyBufferToTexture.bufferImageHeight = bufferImageHeight;
        commandCopyBufferToTexture.textureSubresource = textureSubresource;
        commandCopyBufferToTexture.textureOffset = textureOffset;
        commandCopyBufferToTexture.textureExtent = textureExtent;

        GraphicsCommand command{};
        command.type = GraphicsCommandType_CopyBufferToTexture;
        command.data.commandCopyBufferToTexture = commandCopyBufferToTexture;

        _commands.push_back(command);
    }

    void GraphicsCommandBufferImpl::CopyTextureToBuffer(
        GraphicsTexture* src,
        GraphicsBuffer* dst,
        size_t bufferOffset,
        size_t bufferRowLength,
        size_t bufferImageHeight,
        GraphicsTextureSubresourceRange const& textureSubresource,
        Offset3D const& textureOffset,
        Extent3D const& textureExtent
    )
    {
        GraphicsCommandCopyTextureToBuffer commandCopyTextureToBuffer{};
        commandCopyTextureToBuffer.src = src;
        commandCopyTextureToBuffer.dst = dst;
        commandCopyTextureToBuffer.bufferOffset = bufferOffset;
        commandCopyTextureToBuffer.bufferRowLength = bufferRowLength;
        commandCopyTextureToBuffer.bufferImageHeight = bufferImageHeight;
        commandCopyTextureToBuffer.textureSubresource = textureSubresource;
        commandCopyTextureToBuffer.textureOffset = textureOffset;
        commandCopyTextureToBuffer.textureExtent = textureExtent;

        GraphicsCommand command{};
        command.type = GraphicsCommandType_CopyTextureToBuffer;
        command.data.commandCopyTextureToBuffer = commandCopyTextureToBuffer;

        _commands.push_back(command);
    }

    void GraphicsCommandBufferImpl::CopyTextureToTexture(
        GraphicsTexture* src,
        GraphicsTexture* dst,
        Offset3D const& srcOffset,
        GraphicsTextureSubresourceRange const& srcSubresource,
        Offset3D const& dstOffset,
        GraphicsTextureSubresourceRange const& dstSubresource,
        Extent3D const& extent
    )
    {
        GraphicsCommandCopyTextureToTexture commandCopyTextureToTexture{};
        commandCopyTextureToTexture.src = src;
        commandCopyTextureToTexture.dst = dst;
        commandCopyTextureToTexture.srcOffset = srcOffset;
        commandCopyTextureToTexture.srcSubresource = srcSubresource;
        commandCopyTextureToTexture.dstOffset = dstOffset;
        commandCopyTextureToTexture.dstSubresource = dstSubresource;
        commandCopyTextureToTexture.extent = extent;

        GraphicsCommand command{};
        command.type = GraphicsCommandType_CopyTextureToTexture;
        command.data.commandCopyTextureToTexture = commandCopyTextureToTexture;

        _commands.push_back(command);
    }

    void GraphicsCommandBufferImpl::ClearRenderTarget(
        GraphicsTextureView* textureView,
        GraphicsClearColorValue const& clearValue
    )
    {
        GraphicsCommandClearRenderTarget commandClearRenderTarget{};
        commandClearRenderTarget.textureView = textureView;
        commandClearRenderTarget.clearValue = clearValue;

        GraphicsCommand command{};
        command.type = GraphicsCommandType_ClearRenderTarget;
        command.data.commandClearRenderTarget = commandClearRenderTarget;

        _commands.push_back(command);
    }

    void GraphicsCommandBufferImpl::ClearDepthStencilTarget(
        GraphicsTextureView* textureView,
        GraphicsClearDepthStencilValue const& clearValue
    )
    {
        GraphicsCommandClearDepthStencilTarget commandClearDepthStencilTarget{};
        commandClearDepthStencilTarget.textureView = textureView;
        commandClearDepthStencilTarget.clearValue = clearValue;

        GraphicsCommand command{};
        command.type = GraphicsCommandType_ClearDepthStencilTarget;
        command.data.commandClearDepthStencilTarget = commandClearDepthStencilTarget;

        _commands.push_back(command);
    }

    void GraphicsCommandBufferImpl::BindVertexBuffers(
        size_t firstSlot,
        GraphicsBuffer** buffers,
        size_t bufferCount,
        size_t* offsets
    )
    {
        // Ensure vertex buffer count does not exceed allowed maximum
        bufferCount = std::min(bufferCount, static_cast<size_t>(Graphics::MaxBoundVertexBuffers));

        // Copy over binding data
        std::array<GraphicsBuffer*, Graphics::MaxBoundVertexBuffers> vertexBuffers;
        std::array<size_t, Graphics::MaxBoundVertexBuffers> vertexOffsets;
        for (size_t i = 0; i < bufferCount; i++)
        {
            vertexBuffers[i] = buffers[i];
            vertexOffsets[i] = offsets[i];
        }

        // Set up command state
        GraphicsCommandBindVertexBuffers commandBindVertexBuffers{};
        commandBindVertexBuffers.firsSlot = firstSlot;
        commandBindVertexBuffers.buffers = vertexBuffers;
        commandBindVertexBuffers.bufferCount = bufferCount;
        commandBindVertexBuffers.offsets = vertexOffsets;

        GraphicsCommand command{};
        command.type = GraphicsCommandType_BindVertexBuffers;
        command.data.commandBindVertexBuffers = commandBindVertexBuffers;

        _commands.push_back(command);
    }

    void GraphicsCommandBufferImpl::BindIndexBuffer(
        GraphicsBuffer* buffer,
        size_t offset,
        GraphicsIndexType indexType
    )
    {
        GraphicsCommandBindIndexBuffer commandBindIndexBuffer{};
        commandBindIndexBuffer.buffer = buffer;
        commandBindIndexBuffer.offset = offset;
        commandBindIndexBuffer.indexType = indexType;

        GraphicsCommand command{};
        command.type = GraphicsCommandType_BindIndexBuffer;
        command.data.commandBindIndexBuffer = commandBindIndexBuffer;

        _commands.push_back(command);
    }

    void GraphicsCommandBufferImpl::DrawIndexed(
        uint32_t indexCount,
        uint32_t instanceCount,
        uint32_t firstIndex,
        int32_t vertexOffset,
        uint32_t firstInstance
    )
    {
        GraphicsCommandDrawIndexed commandDrawIndexed{};
        commandDrawIndexed.indexCount = indexCount;
        commandDrawIndexed.instanceCount = instanceCount;
        commandDrawIndexed.firstIndex = firstIndex;
        commandDrawIndexed.vertexOffset = vertexOffset;
        commandDrawIndexed.firstInstance = firstInstance;

        GraphicsCommand command{};
        command.type = GraphicsCommandType_DrawIndexed;
        command.data.commandDrawIndexed = commandDrawIndexed;

        _commands.push_back(command);
    }

    void GraphicsCommandBufferImpl::DrawIndexedIndirect(
        GraphicsBuffer* buffer,
        size_t offset,
        uint32_t drawCount,
        uint32_t stride
    )
    {
        GraphicsCommandDrawIndexedIndirect commandDrawIndexedIndirect{};
        commandDrawIndexedIndirect.buffer = buffer;
        commandDrawIndexedIndirect.offset = offset;
        commandDrawIndexedIndirect.drawCount = drawCount;
        commandDrawIndexedIndirect.stride = stride;

        GraphicsCommand command{};
        command.type = GraphicsCommandType_DrawIndexedIndirect;
        command.data.commandDrawIndexedIndirect = commandDrawIndexedIndirect;

        _commands.push_back(command);
    }

    void GraphicsCommandBufferImpl::Dispatch(
        uint32_t sizeX,
        uint32_t sizeY,
        uint32_t sizeZ
    )
    {
        GraphicsCommandDispatch commandDispatch{};
        commandDispatch.sizeX = sizeX;
        commandDispatch.sizeY = sizeY;
        commandDispatch.sizeZ = sizeZ;

        GraphicsCommand command{};
        command.type = GraphicsCommandType_Dispatch;
        command.data.commandDispatch = commandDispatch;

        _commands.push_back(command);
    }

    void GraphicsCommandBufferImpl::DispatchIndirect(
        GraphicsBuffer* buffer,
        size_t offset
    )
    {
        GraphicsCommandDispatchIndirect commandDispatchIndirect{};
        commandDispatchIndirect.buffer = buffer;
        commandDispatchIndirect.offset = offset;

        GraphicsCommand command{};
        command.type = GraphicsCommandType_DispatchIndirect;
        command.data.commandDispatchIndirect = commandDispatchIndirect;

        _commands.push_back(command);
    }

    void GraphicsCommandBufferImpl::BeginRendering(
        GraphicsRenderAttachmentInfo* renderAttachments,
        size_t renderAttachmentCount,
        GraphicsRenderAttachmentInfo* depthStencilAttachment
    )
    {
        // Ensure render attachment count does not exceed allowed maximum
        renderAttachmentCount = std::min(renderAttachmentCount, static_cast<size_t>(Graphics::MaxBoundRenderTargets));

        // Copy over render attachment data
        std::array<GraphicsRenderAttachmentInfo, Graphics::MaxBoundRenderTargets> boundRenderAttachments;
        for (size_t i = 0; i < renderAttachmentCount; i++) {
            boundRenderAttachments[i] = renderAttachments[i];
        }

        // Set up command state
        GraphicsCommandBeginRendering commandBeginRendering{};
        commandBeginRendering.renderAttachments = boundRenderAttachments;
        commandBeginRendering.renderAttachmentCount = renderAttachmentCount;
        if (depthStencilAttachment != nullptr)
        {
            commandBeginRendering.hasDepthStencilAttachment = true;
            commandBeginRendering.depthStencilAttachment = *depthStencilAttachment;
        }

        GraphicsCommand command{};
        command.type = GraphicsCommandType_BeginRendering;
        command.data.commandBeginRendering = commandBeginRendering;

        _commands.push_back(command);
    }

    void GraphicsCommandBufferImpl::EndRendering()
    {
        GraphicsCommand command{};
        command.type = GraphicsCommandType_EndRendering;

        _commands.push_back(command);
    }
} // namespace Bonsai
