#include "VulkanCommandRecorder.hpp"

#include <cassert>
#include <volk.h>
#include "Core/Logger.hpp"
#include "Gfx/Internal/GraphicsCommandBufferInternal.hpp"
#include "Gfx/Vulkan/VulkanGraphicsHelpers.hpp"
#include "Gfx/Vulkan/VulkanGraphicsTypes.hpp"

namespace Bonsai
{
    void VulkanCommandRecorder::RecordCommandBuffer(VkCommandBuffer commandBuffer, GraphicsCommandBufferImpl const* recordedCommands)
    {
        assert(commandBuffer != VK_NULL_HANDLE && "commandBuffer was VK_NULL_HANDLE!");
        assert(recordedCommands != nullptr && "recordedCommands was nullptr!");

        for (auto const& command : recordedCommands->GetRecordedCommands()) {
            RecordCommand(commandBuffer, command);
        }
    }

    void VulkanCommandRecorder::RecordCommand(VkCommandBuffer commandBuffer, GraphicsCommand const& command)
    {
        switch (command.type)
        {
        case GraphicsCommandType_Undefined: {
            SPDLOG_WARN("Encountered GraphicsCommandType_Undefined!");
        } break;
        case GraphicsCommandType_PipelineBarrier: {
            GraphicsCommandPipelineBarrier const& commandData = command.data.commandPipelineBarrier;

            // Set up barriers
            std::array<VkMemoryBarrier2, Graphics::MaxGlobalBarriers> memoryBarriers;
            for (size_t i = 0; i < commandData.globalBarrierCount; i++)
            {
                GraphicsBarrier const& graphicsBarrier = commandData.globalBarriers[i];
                VulkanResourceAccessState const srcAccessState = GetVulkanResourceAccessState(graphicsBarrier.srcResourceState);
                VulkanResourceAccessState const dstAccessState = GetVulkanResourceAccessState(graphicsBarrier.dstResourceState);

                VkMemoryBarrier2& memoryBarrier = memoryBarriers[i];
                memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2;
                memoryBarrier.pNext = nullptr;
                memoryBarrier.srcStageMask = srcAccessState.stageMask;
                memoryBarrier.srcAccessMask = srcAccessState.accessMask;
                memoryBarrier.dstStageMask = dstAccessState.stageMask;
                memoryBarrier.dstAccessMask = dstAccessState.accessMask;
            }

            std::array<VkBufferMemoryBarrier2, Graphics::MaxBufferBarriers> bufferMemoryBarriers;
            for (size_t i = 0; i < commandData.bufferBarrierCount; i++)
            {
                GraphicsBufferBarrier const& graphicsBufferBarrier = commandData.bufferBarriers[i];
                VulkanResourceAccessState const srcAccessState = GetVulkanResourceAccessState(graphicsBufferBarrier.srcResourceState);
                VulkanResourceAccessState const dstAccessState = GetVulkanResourceAccessState(graphicsBufferBarrier.dstResourceState);
                auto* vulkanBuffer = static_cast<VulkanBuffer*>(graphicsBufferBarrier.buffer);

                VkBufferMemoryBarrier2& bufferMemoryBarrier = bufferMemoryBarriers[i];
                bufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
                bufferMemoryBarrier.pNext = nullptr;
                bufferMemoryBarrier.srcStageMask = srcAccessState.stageMask;
                bufferMemoryBarrier.srcAccessMask = srcAccessState.accessMask;
                bufferMemoryBarrier.dstStageMask = dstAccessState.stageMask;
                bufferMemoryBarrier.dstAccessMask = dstAccessState.accessMask;
                bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                bufferMemoryBarrier.buffer = vulkanBuffer->GetBuffer();
                bufferMemoryBarrier.offset = graphicsBufferBarrier.offset;
                bufferMemoryBarrier.size = graphicsBufferBarrier.size;
            }

            std::array<VkImageMemoryBarrier2, Graphics::MaxTextureBarriers> imageMemoryBarriers;
            for (size_t i = 0; i < commandData.textureBarrierCount; i++)
            {
                GraphicsTextureBarrier const& graphicsTextureBarrier = commandData.textureBarriers[i];
                VulkanResourceAccessState const srcAccessState = GetVulkanResourceAccessState(graphicsTextureBarrier.srcResourceState);
                VulkanResourceAccessState const dstAccessState = GetVulkanResourceAccessState(graphicsTextureBarrier.dstResourceState);
                auto* vulkanTexture = static_cast<VulkanTexture*>(graphicsTextureBarrier.texture);

                VkImageSubresourceRange subresourceRange{};
                subresourceRange.aspectMask = VulkanGraphicsHelpers::GetVulkanImageAspectFlags(graphicsTextureBarrier.subresource.aspectMask);
                subresourceRange.baseMipLevel = graphicsTextureBarrier.subresource.baseMipLevel;
                subresourceRange.levelCount = graphicsTextureBarrier.subresource.levelCount;
                subresourceRange.baseArrayLayer = graphicsTextureBarrier.subresource.baseArrayLayer;
                subresourceRange.layerCount = graphicsTextureBarrier.subresource.layerCount;

                VkImageMemoryBarrier2& imageMemoryBarrier = imageMemoryBarriers[i];
                imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
                imageMemoryBarrier.pNext = nullptr;
                imageMemoryBarrier.srcStageMask = srcAccessState.stageMask;
                imageMemoryBarrier.srcAccessMask = srcAccessState.accessMask;
                imageMemoryBarrier.dstStageMask = dstAccessState.stageMask;
                imageMemoryBarrier.dstAccessMask = dstAccessState.accessMask;
                imageMemoryBarrier.oldLayout = VulkanGraphicsHelpers::GetVulkanImageLayout(graphicsTextureBarrier.srcLayout);
                imageMemoryBarrier.newLayout = VulkanGraphicsHelpers::GetVulkanImageLayout(graphicsTextureBarrier.dstLayout);
                imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                imageMemoryBarrier.image = vulkanTexture->GetImage();
                imageMemoryBarrier.subresourceRange = subresourceRange;
            }

            // Set up dependency info
            VkDependencyInfo dependencyInfo{};
            dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
            dependencyInfo.pNext = nullptr;
            dependencyInfo.dependencyFlags = 0;
            dependencyInfo.memoryBarrierCount = static_cast<uint32_t>(commandData.globalBarrierCount);
            dependencyInfo.pMemoryBarriers = memoryBarriers.data();
            dependencyInfo.bufferMemoryBarrierCount = static_cast<uint32_t>(commandData.bufferBarrierCount);
            dependencyInfo.pBufferMemoryBarriers = bufferMemoryBarriers.data();
            dependencyInfo.imageMemoryBarrierCount = static_cast<uint32_t>(commandData.textureBarrierCount);
            dependencyInfo.pImageMemoryBarriers = imageMemoryBarriers.data();

            vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
        } break;
        case GraphicsCommandType_CopyBufferToBuffer: {
            GraphicsCommandCopyBufferToBuffer const& commandData = command.data.commandCopyBufferToBuffer;

            auto* srcBuffer = static_cast<VulkanBuffer*>(commandData.src);
            auto* dstBuffer = static_cast<VulkanBuffer*>(commandData.dst);

            VkBufferCopy copyRegion{};
            copyRegion.size = commandData.size;
            copyRegion.srcOffset = commandData.srcOffset;
            copyRegion.dstOffset = commandData.dstOffset;

            vkCmdCopyBuffer(
                commandBuffer,
                srcBuffer->GetBuffer(),
                dstBuffer->GetBuffer(),
                1,
                &copyRegion
            );
        } break;
        case GraphicsCommandType_CopyBufferToTexture: {
            GraphicsCommandCopyBufferToTexture const& commandData = command.data.commandCopyBufferToTexture;

            auto* srcBuffer = static_cast<VulkanBuffer*>(commandData.src);
            auto* dstTexture = static_cast<VulkanTexture*>(commandData.dst);

            VkImageSubresourceLayers subresource{};
            subresource.aspectMask = VulkanGraphicsHelpers::GetVulkanImageAspectFlags(commandData.textureSubresource.aspectMask);
            subresource.mipLevel = commandData.textureSubresource.baseMipLevel;
            subresource.baseArrayLayer = commandData.textureSubresource.baseArrayLayer;
            subresource.layerCount = commandData.textureSubresource.layerCount;

            VkBufferImageCopy copyRegion{};
            copyRegion.bufferOffset = static_cast<uint32_t>(commandData.bufferOffset);
            copyRegion.bufferRowLength = static_cast<uint32_t>(commandData.bufferRowLength);
            copyRegion.bufferImageHeight = static_cast<uint32_t>(commandData.bufferImageHeight);
            copyRegion.imageSubresource = subresource;
            copyRegion.imageOffset = VkOffset3D{ commandData.textureOffset.x, commandData.textureOffset.y, commandData.textureOffset.z };
            copyRegion.imageExtent = VkExtent3D{ commandData.textureExtent.width, commandData.textureExtent.height, commandData.textureExtent.depth };

            vkCmdCopyBufferToImage(
                commandBuffer,
                srcBuffer->GetBuffer(),
                dstTexture->GetImage(),
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &copyRegion
            );
        } break;
        case GraphicsCommandType_CopyTextureToBuffer: {
            GraphicsCommandCopyTextureToBuffer const& commandData = command.data.commandCopyTextureToBuffer;

            auto* srcTexture = static_cast<VulkanTexture*>(commandData.src);
            auto* dstBuffer = static_cast<VulkanBuffer*>(commandData.dst);

            VkImageSubresourceLayers subresource{};
            subresource.aspectMask = VulkanGraphicsHelpers::GetVulkanImageAspectFlags(commandData.textureSubresource.aspectMask);
            subresource.mipLevel = commandData.textureSubresource.baseMipLevel;
            subresource.baseArrayLayer = commandData.textureSubresource.baseArrayLayer;
            subresource.layerCount = commandData.textureSubresource.layerCount;

            VkBufferImageCopy copyRegion{};
            copyRegion.bufferOffset = static_cast<uint32_t>(commandData.bufferOffset);
            copyRegion.bufferRowLength = static_cast<uint32_t>(commandData.bufferRowLength);
            copyRegion.bufferImageHeight = static_cast<uint32_t>(commandData.bufferImageHeight);
            copyRegion.imageSubresource = subresource;
            copyRegion.imageOffset = VkOffset3D{ commandData.textureOffset.x, commandData.textureOffset.y, commandData.textureOffset.z };
            copyRegion.imageExtent = VkExtent3D{ commandData.textureExtent.width, commandData.textureExtent.height, commandData.textureExtent.depth };

            vkCmdCopyImageToBuffer(
                commandBuffer,
                srcTexture->GetImage(),
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                dstBuffer->GetBuffer(),
                1,
                &copyRegion
            );
        } break;
        case GraphicsCommandType_CopyTextureToTexture: {
            GraphicsCommandCopyTextureToTexture const& commandData = command.data.commandCopyTextureToTexture;

            auto* srcTexture = static_cast<VulkanTexture*>(commandData.src);
            auto* dstTexture = static_cast<VulkanTexture*>(commandData.dst);

            VkImageSubresourceLayers srcSubresource{};
            srcSubresource.aspectMask = VulkanGraphicsHelpers::GetVulkanImageAspectFlags(commandData.srcSubresource.aspectMask);
            srcSubresource.mipLevel = commandData.srcSubresource.baseMipLevel;
            srcSubresource.baseArrayLayer = commandData.srcSubresource.baseArrayLayer;
            srcSubresource.layerCount = commandData.srcSubresource.layerCount;

            VkImageSubresourceLayers dstSubresource{};
            dstSubresource.aspectMask = VulkanGraphicsHelpers::GetVulkanImageAspectFlags(commandData.dstSubresource.aspectMask);
            dstSubresource.mipLevel = commandData.dstSubresource.baseMipLevel;
            dstSubresource.baseArrayLayer = commandData.dstSubresource.baseArrayLayer;
            dstSubresource.layerCount = commandData.dstSubresource.layerCount;

            VkImageCopy copyRegion{};
            copyRegion.srcOffset = VkOffset3D{ commandData.srcOffset.x, commandData.srcOffset.y, commandData.srcOffset.z };
            copyRegion.srcSubresource = srcSubresource;
            copyRegion.dstOffset = VkOffset3D{ commandData.dstOffset.x, commandData.dstOffset.y, commandData.dstOffset.z };
            copyRegion.dstSubresource = dstSubresource;
            copyRegion.extent = VkExtent3D{ commandData.extent.width, commandData.extent.height, commandData.extent.depth };

            vkCmdCopyImage(
                commandBuffer,
                srcTexture->GetImage(),
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                dstTexture->GetImage(),
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &copyRegion
            );
        } break;
        case GraphicsCommandType_ClearRenderTarget: {
            GraphicsCommandClearRenderTarget const& commandData = command.data.commandClearRenderTarget;

            auto* textureView = static_cast<VulkanTextureView*>(commandData.textureView);
            auto* parentTexture = static_cast<VulkanTexture*>(textureView->GetParentTexture());

            VkClearColorValue const clearColor{{
                commandData.clearValue.float32[0],
                commandData.clearValue.float32[1],
                commandData.clearValue.float32[2],
                commandData.clearValue.float32[3],
            }};

            VkImageSubresourceRange subresource{};
            subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            subresource.baseMipLevel = 0;
            subresource.levelCount = VK_REMAINING_MIP_LEVELS;
            subresource.baseArrayLayer = 0;
            subresource.layerCount = VK_REMAINING_ARRAY_LAYERS;

            vkCmdClearColorImage(
                commandBuffer,
                parentTexture->GetImage(),
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                &clearColor,
                1,
                &subresource
            );
        } break;
        case GraphicsCommandType_ClearDepthStencilTarget: {
            GraphicsCommandClearDepthStencilTarget const& commandData = command.data.commandClearDepthStencilTarget;

            auto* textureView = static_cast<VulkanTextureView*>(commandData.textureView);
            auto* parentTexture = static_cast<VulkanTexture*>(textureView->GetParentTexture());
            GraphicsTextureDesc const parentTextureDesc = parentTexture->GetDesc();

            VkClearDepthStencilValue const clearDepthStencil{
                commandData.clearValue.depth,
                commandData.clearValue.stencil,
            };

            GraphicsTextureAspectFlags const aspectMask = GraphicsHelpers::GetGraphicsTextureAspectFlags(parentTextureDesc.textureFormat);
            VkImageSubresourceRange subresource{};
            subresource.aspectMask = VulkanGraphicsHelpers::GetVulkanImageAspectFlags(aspectMask);
            subresource.baseMipLevel = 0;
            subresource.levelCount = VK_REMAINING_MIP_LEVELS;
            subresource.baseArrayLayer = 0;
            subresource.layerCount = VK_REMAINING_ARRAY_LAYERS;

            vkCmdClearDepthStencilImage(
                commandBuffer,
                parentTexture->GetImage(),
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                &clearDepthStencil,
                1,
                &subresource
            );
        } break;
        case GraphicsCommandType_BindVertexBuffers: {
            GraphicsCommandBindVertexBuffers const& commandData = command.data.commandBindVertexBuffers;
            
            std::array<VkBuffer, Graphics::MaxBoundVertexBuffers> vertexBuffers;
            for (size_t i = 0; i < commandData.bufferCount; i++) {
                auto* vulkanBuffer = static_cast<VulkanBuffer*>(commandData.buffers[i]);
                vertexBuffers[i] = vulkanBuffer->GetBuffer();
            }

            vkCmdBindVertexBuffers(
                commandBuffer,
                static_cast<uint32_t>(commandData.firsSlot),
                static_cast<uint32_t>(commandData.bufferCount),
                vertexBuffers.data(),
                commandData.offsets.data()
            );
        } break;
        case GraphicsCommandType_BindIndexBuffer: {
            GraphicsCommandBindIndexBuffer const& commandData = command.data.commandBindIndexBuffer;

            auto* vulkanBuffer = static_cast<VulkanBuffer*>(commandData.buffer);
            vkCmdBindIndexBuffer(
                commandBuffer,
                vulkanBuffer->GetBuffer(),
                commandData.offset,
                VulkanGraphicsHelpers::GetVulkanIndexType(commandData.indexType)
            );
        } break;
        case GraphicsCommandType_DrawIndexed: {
            GraphicsCommandDrawIndexed const& commandData = command.data.commandDrawIndexed;

            vkCmdDrawIndexed(
                commandBuffer,
                commandData.indexCount,
                commandData.instanceCount,
                commandData.firstIndex,
                commandData.vertexOffset,
                commandData.firstInstance
            );
        } break;
        case GraphicsCommandType_DrawIndexedIndirect: {
            GraphicsCommandDrawIndexedIndirect const& commandData = command.data.commandDrawIndexedIndirect;

            auto* vulkanBuffer = static_cast<VulkanBuffer*>(commandData.buffer);
            vkCmdDrawIndexedIndirect(
                commandBuffer,
                vulkanBuffer->GetBuffer(),
                commandData.offset,
                commandData.drawCount,
                commandData.stride
            );
        } break;
        case GraphicsCommandType_Dispatch: {
            GraphicsCommandDispatch const& commandData = command.data.commandDispatch;

            vkCmdDispatch(
                commandBuffer,
                commandData.sizeX,
                commandData.sizeY,
                commandData.sizeZ
            );
        } break;
        case GraphicsCommandType_DispatchIndirect: {
            GraphicsCommandDispatchIndirect const& commandData = command.data.commandDispatchIndirect;

            auto* vulkanBuffer = static_cast<VulkanBuffer*>(commandData.buffer);
            vkCmdDispatchIndirect(
                commandBuffer,
                vulkanBuffer->GetBuffer(),
                commandData.offset
            );
        } break;
        case GraphicsCommandType_BeginRendering: {
            GraphicsCommandBeginRendering const& commandData = command.data.commandBeginRendering;

            // Set up color targets
            std::array<VkRenderingAttachmentInfo, Graphics::MaxBoundRenderTargets> colorAttachments;
            for (size_t i = 0; i < commandData.renderAttachmentCount; i++)
            {
                GraphicsRenderAttachmentInfo const& renderAttachmentInfo = commandData.renderAttachments[i];
                auto* vulkanRenderTarget = static_cast<VulkanTextureView*>(renderAttachmentInfo.renderTarget);
                auto* vulkanResolveTarget = static_cast<VulkanTextureView*>(renderAttachmentInfo.resolveTarget);

                VkRenderingAttachmentInfo& colorAttachment = colorAttachments[i];
                colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
                colorAttachment.pNext = nullptr;
                colorAttachment.imageView = vulkanRenderTarget->GetImageView();
                colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                colorAttachment.resolveMode = vulkanResolveTarget != nullptr ? VK_RESOLVE_MODE_AVERAGE_BIT : VK_RESOLVE_MODE_NONE;
                colorAttachment.resolveImageView = vulkanResolveTarget != nullptr ? vulkanResolveTarget->GetImageView() : VK_NULL_HANDLE;
                colorAttachment.resolveImageLayout = vulkanResolveTarget != nullptr ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED;
                colorAttachment.loadOp = VulkanGraphicsHelpers::GetVulkanAttachmentLoadOp(renderAttachmentInfo.loadOp);
                colorAttachment.storeOp = VulkanGraphicsHelpers::GetVulkanAttachmentStoreOp(renderAttachmentInfo.storeOp);
                colorAttachment.clearValue.color = VkClearColorValue{{
                    renderAttachmentInfo.clearValue.color.float32[0],
                    renderAttachmentInfo.clearValue.color.float32[1],
                    renderAttachmentInfo.clearValue.color.float32[2],
                    renderAttachmentInfo.clearValue.color.float32[3],
                }};
            }

            // Set up optional depth/stencil attachment
            VkRenderingAttachmentInfo depthStencilAttachment{};
            if (commandData.hasDepthStencilAttachment)
            {
                GraphicsRenderAttachmentInfo const& depthStencilAttachmentInfo = commandData.depthStencilAttachment;
                auto* vulkanRenderTarget = static_cast<VulkanTextureView*>(depthStencilAttachmentInfo.renderTarget);
                auto* vulkanResolveTarget = static_cast<VulkanTextureView*>(depthStencilAttachmentInfo.renderTarget);

                depthStencilAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
                depthStencilAttachment.pNext = nullptr;
                depthStencilAttachment.imageView = vulkanRenderTarget->GetImageView();
                depthStencilAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                depthStencilAttachment.resolveMode = vulkanResolveTarget != nullptr ? VK_RESOLVE_MODE_AVERAGE_BIT : VK_RESOLVE_MODE_NONE;
                depthStencilAttachment.resolveImageView = vulkanResolveTarget != nullptr ? vulkanResolveTarget->GetImageView() : VK_NULL_HANDLE;
                depthStencilAttachment.resolveImageLayout = vulkanResolveTarget != nullptr ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED;
                depthStencilAttachment.loadOp = VulkanGraphicsHelpers::GetVulkanAttachmentLoadOp(depthStencilAttachmentInfo.loadOp);
                depthStencilAttachment.storeOp = VulkanGraphicsHelpers::GetVulkanAttachmentStoreOp(depthStencilAttachmentInfo.storeOp);
                depthStencilAttachment.clearValue.depthStencil = VkClearDepthStencilValue{
                    depthStencilAttachmentInfo.clearValue.depthStencil.depth,
                    depthStencilAttachmentInfo.clearValue.depthStencil.stencil,
                };
            }

            // Set up render area
            VkRect2D renderArea{};
            renderArea.offset = VkOffset2D{ commandData.renderArea.offset.x, commandData.renderArea.offset.y };
            renderArea.extent = VkExtent2D{ commandData.renderArea.extent.width, commandData.renderArea.extent.height };

            // Set up rendering info
            VkRenderingInfo renderingInfo{};
            renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
            renderingInfo.pNext = nullptr;
            renderingInfo.flags = 0;
            renderingInfo.renderArea = renderArea;
            renderingInfo.layerCount = 1;
            renderingInfo.viewMask = 0;
            renderingInfo.colorAttachmentCount = commandData.renderAttachmentCount;
            renderingInfo.pColorAttachments = colorAttachments.data();
            renderingInfo.pDepthAttachment = nullptr;
            renderingInfo.pStencilAttachment = nullptr;

            if (commandData.hasDepthStencilAttachment)
            {
                auto* vulkanRenderTarget = static_cast<VulkanTextureView*>(commandData.depthStencilAttachment.renderTarget);
                GraphicsTextureViewDesc const renderTargetDesc = vulkanRenderTarget->GetDesc();

                renderingInfo.pDepthAttachment = GraphicsHelpers::IsDepthFormat(renderTargetDesc.viewFormat) ? &depthStencilAttachment : nullptr;
                renderingInfo.pStencilAttachment = GraphicsHelpers::IsStencilFormat(renderTargetDesc.viewFormat) ? &depthStencilAttachment : nullptr;
            }

            vkCmdBeginRendering(commandBuffer, &renderingInfo);
        } break;
        case GraphicsCommandType_EndRendering: {
            vkCmdEndRendering(commandBuffer);
        } break;
        default: {
            SPDLOG_WARN("Unknown GraphicsCommandType value: {}", static_cast<uint32_t>(command.type));
        } break;
        }
    }

    VulkanResourceAccessState VulkanCommandRecorder::GetVulkanResourceAccessState(GraphicsResourceState const& resourceState)
    {
        switch (resourceState)
        {
        case GraphicsResourceState_Common:
            return { VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT, VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT };
        case GraphicsResourceState_IndirectBuffer_Read:
            return { VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT, VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT };
        case GraphicsResourceState_IndexBuffer_Read:
            return { VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT, VK_ACCESS_2_INDEX_READ_BIT };
        case GraphicsResourceState_VertexBuffer_Read:
            return { VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT, VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT };
        case GraphicsResourceState_VertexShader_Read:
            return { VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT, VK_ACCESS_2_UNIFORM_READ_BIT | VK_ACCESS_2_SHADER_READ_BIT };
        case GraphicsResourceState_VertexShader_Write:
            return { VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT, VK_ACCESS_2_SHADER_WRITE_BIT };
        case GraphicsResourceState_PixelShader_Read:
            return { VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, VK_ACCESS_2_UNIFORM_READ_BIT | VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT };
        case GraphicsResourceState_PixelShader_Write:
            return { VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, VK_ACCESS_2_SHADER_WRITE_BIT };
        case GraphicsResourceState_ComputeShader_Read:
            return { VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT, VK_ACCESS_2_UNIFORM_READ_BIT | VK_ACCESS_2_SHADER_READ_BIT };
        case GraphicsResourceState_ComputeShader_Write:
            return { VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT, VK_ACCESS_2_SHADER_WRITE_BIT };
        case GraphicsResourceState_RenderTarget_Read:
            return { VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT_KHR };
        case GraphicsResourceState_RenderTarget_Write:
            return { VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT_KHR };
        case GraphicsResourceState_DepthStencilTarget_Read:
            return { VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT, VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT };
        case GraphicsResourceState_DepthStencilTarget_Write:
            return { VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT, VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT };
        case GraphicsResourceState_Transfer_Read:
            return { VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_ACCESS_2_TRANSFER_READ_BIT };
        case GraphicsResourceState_Transfer_Write:
            return { VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT };
        case GraphicsResourceState_Host_Read:
            return { VK_PIPELINE_STAGE_2_HOST_BIT, VK_ACCESS_2_MEMORY_READ_BIT };
        case GraphicsResourceState_Host_Write:
            return { VK_PIPELINE_STAGE_2_HOST_BIT, VK_ACCESS_2_MEMORY_WRITE_BIT };
        case GraphicsResourceState_Present:
            return { VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT, VK_ACCESS_2_NONE };
        default:
            break;
        }

        return { 0, 0 };
    }
} // namespace Bonsai
