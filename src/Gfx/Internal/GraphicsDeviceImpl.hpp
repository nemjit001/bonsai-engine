#pragma once
#ifndef BONSAI_GRAPHICS_DEVICE_IMPL_HPP
#define BONSAI_GRAPHICS_DEVICE_IMPL_HPP

#include <memory>
#include "Gfx/GraphicsDevice.hpp"
#include "Gfx/Internal/GraphicsCommandBufferPool.hpp"

namespace Bonsai
{
    class GraphicsDeviceImpl : public GraphicsDevice
    {
    public:
        GraphicsDeviceImpl();

        void DestroyBuffer(GraphicsBuffer* buffer) override;
        void DestroyTexture(GraphicsTexture* texture) override;
        void DestroyTextureView(GraphicsTextureView* textureView) override;
        void DestroySampler(GraphicsSampler* sampler) override;
        void DestroyDescriptorSetLayout(GraphicsDescriptorSetLayout* descriptorSetLayout) override;
        void DestroyDescriptorSet(GraphicsDescriptorSet* descriptorSet) override;
        void DestroyPipelineLayout(GraphicsPipelineLayout* pipelineLayout) override;
        void DestroyRenderPipeline(GraphicsRenderPipeline* renderPipeline) override;
        void DestroyComputePipeline(GraphicsComputePipeline* computePipeline) override;
        void DestroySwapchain(GraphicsSwapchain* swapchain) override;

        void CopyToBuffer(GraphicsBuffer* buffer, void const* data, size_t size, size_t offset) override;
        void CopyFromBuffer(GraphicsBuffer* buffer, void* outData, size_t size, size_t offset) override;

        GraphicsCommandBuffer* BeginCommandBuffer() override;
        void SubmitFrameCommands(FrameContext* frameContext, GraphicsCommandBuffer* commandBuffer) override;
        void SubmitTransferCommands(TransferBatch* transferBatch, GraphicsCommandBuffer* commandBuffer) override;

        GraphicsTexture* GetSwapchainBackbuffer(GraphicsSwapchain* swapchain) override;
        GraphicsTextureView* GetSwapchainBackbufferView(GraphicsSwapchain* swapchain) override;
        Extent2D GetSwapchainExtent(GraphicsSwapchain* swapchain) override;

    private:
        std::unique_ptr<GraphicsCommandBufferPool> _commandBufferPool;
    };
} // namespace Bonsai

#endif //BONSAI_GRAPHICS_DEVICE_IMPL_HPP
