#include "GraphicsDeviceImpl.hpp"

#include <cstring>
#include "Gfx/Internal/GraphicsTypesInternal.hpp"

namespace Bonsai
{
    GraphicsDeviceImpl::GraphicsDeviceImpl()
        :
        _commandBufferPool(std::make_unique<GraphicsCommandBufferPool>())
    {
        //
    }

    GraphicsTextureView* GraphicsDeviceImpl::CreateDefaultTextureView(GraphicsTexture* texture)
    {
        if (texture == nullptr) {
            return nullptr;
        }

        GraphicsTextureDesc const& textureDesc = texture->GetDesc();

        GraphicsTextureSubresourceRange subresourceRange{};
        subresourceRange.aspectMask = GraphicsHelpers::GetGraphicsTextureAspectFlags(textureDesc.textureFormat);
        subresourceRange.baseMipLevel = 0;
        subresourceRange.levelCount = Graphics::RemainingMipLevels;
        subresourceRange.baseArrayLayer = 0;
        subresourceRange.layerCount = Graphics::RemainingArrayLayers;

        GraphicsTextureViewDesc textureViewDesc{};
        textureViewDesc.texture = texture;
        textureViewDesc.viewFormat = textureDesc.textureFormat;
        textureViewDesc.viewType = GraphicsHelpers::GetGraphicsTextureViewType(textureDesc.textureType, textureDesc.depthOrLayers);
        textureViewDesc.subresourceRange = subresourceRange;

        return CreateTextureView(textureViewDesc);
    }

    void GraphicsDeviceImpl::DestroyBuffer(GraphicsBuffer* buffer)
    {
        if (buffer != nullptr) {
            buffer->ReleaseRef();
        }
    }

    void GraphicsDeviceImpl::DestroyTexture(GraphicsTexture* texture)
    {
        if (texture != nullptr) {
            texture->ReleaseRef();
        }
    }
    
    void GraphicsDeviceImpl::DestroyTextureView(GraphicsTextureView* textureView)
    {
        if (textureView != nullptr) {
            textureView->ReleaseRef();
        }
    }

    void GraphicsDeviceImpl::DestroySampler(GraphicsSampler* sampler)
    {
        if (sampler != nullptr) {
            sampler->ReleaseRef();
        }
    }

    void GraphicsDeviceImpl::DestroyDescriptorSetLayout(GraphicsDescriptorSetLayout* descriptorSetLayout)
    {
        if (descriptorSetLayout != nullptr) {
            descriptorSetLayout->ReleaseRef();
        }
    }

    void GraphicsDeviceImpl::DestroyDescriptorSet(GraphicsDescriptorSet* descriptorSet)
    {
        if (descriptorSet != nullptr) {
            descriptorSet->ReleaseRef();
        }
    }

    void GraphicsDeviceImpl::DestroyPipelineLayout(GraphicsPipelineLayout* pipelineLayout)
    {
        if (pipelineLayout != nullptr) {
            pipelineLayout->ReleaseRef();
        }
    }

    void GraphicsDeviceImpl::DestroyRenderPipeline(GraphicsRenderPipeline* renderPipeline)
    {
        if (renderPipeline != nullptr) {
            renderPipeline->ReleaseRef();
        }
    }

    void GraphicsDeviceImpl::DestroyComputePipeline(GraphicsComputePipeline* computePipeline)
    {
        if (computePipeline != nullptr) {
            computePipeline->ReleaseRef();
        }
    }

    void GraphicsDeviceImpl::DestroySwapchain(GraphicsSwapchain* swapchain)
    {
        if (swapchain != nullptr) {
            swapchain->ReleaseRef();
        }
    }

    void GraphicsDeviceImpl::CopyToBuffer(GraphicsBuffer* buffer, void const* data, size_t size, size_t offset)
    {
        // Get mapped buffer pointer
        void* bufferData = buffer->GetMappedMemory();
        if (bufferData == nullptr) {
            return;
        }

        // Copy data to buffer
        bufferData = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(bufferData) + offset);
        std::memcpy(bufferData, data, size);
    }

    void GraphicsDeviceImpl::CopyFromBuffer(GraphicsBuffer* buffer, void* outData, size_t size, size_t offset)
    {
        // Check output pointer
        if (outData == nullptr) {
            return;
        }

        // Get mapped buffer pointer
        void* bufferData = buffer->GetMappedMemory();
        if (bufferData == nullptr) {
            return;
        }

        // Copy data from buffer
        bufferData = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(bufferData) + offset);
        std::memcpy(outData, bufferData, size);
    }

    GraphicsCommandBuffer* GraphicsDeviceImpl::BeginCommandBuffer()
    {
        return _commandBufferPool->BeginNewCommandBuffer();
    }

    void GraphicsDeviceImpl::SubmitFrameCommands(FrameContext* frameContext, GraphicsCommandBuffer* commandBuffer)
    {
        frameContext->commandBuffers.push_back(commandBuffer);
    }

    void GraphicsDeviceImpl::SubmitTransferCommands(TransferBatch* transferBatch, GraphicsCommandBuffer* commandBuffer)
    {
        transferBatch->commandBuffers.push_back(commandBuffer);
    }

    GraphicsTexture* GraphicsDeviceImpl::GetSwapchainBackbuffer(GraphicsSwapchain* swapchain)
    {
        if (swapchain == nullptr) {
            return nullptr;
        }

        // Return the backbuffer texture
        GraphicsTexture* backbuffer = swapchain->GetBackbuffer();
        backbuffer->AddRef();
        return backbuffer;
    }

    GraphicsTextureView* GraphicsDeviceImpl::GetSwapchainBackbufferView(GraphicsSwapchain* swapchain)
    {
        if (swapchain == nullptr) {
            return nullptr;
        }

        // Return the backbuffer view
        return swapchain->GetBackbufferView();
    }

    GraphicsFormat GraphicsDeviceImpl::GetSwapchainFormat(GraphicsSwapchain* swapchain)
    {
        if (swapchain == nullptr) {
            return GraphicsFormat_Unknown;
        }

        return swapchain->GetFormat();
    }

    Extent2D GraphicsDeviceImpl::GetSwapchainExtent(GraphicsSwapchain* swapchain)
    {
        if (swapchain == nullptr) {
            return { 0, 0 };
        }

        return swapchain->GetExtent();
    }
} // namespace Bonsai
