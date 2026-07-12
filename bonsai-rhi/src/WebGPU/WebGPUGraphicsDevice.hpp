#pragma once
#ifndef WEBGPU_GRAPHICS_DEVICE_HPP
#define WEBGPU_GRAPHICS_DEVICE_HPP
#if BONSAI_RHI_WEBGPU_SUPPORT

#include <webgpu/webgpu.h>
#include "BonsaiRHI.hpp"
#include "BonsaiRHITypes.hpp"

namespace RHI
{
    /// @brief WebGPU command buffer implementation.
    class WebGPUCommandBuffer : public IGPUCommandBuffer
    {
    public:
        explicit WebGPUCommandBuffer(WGPUCommandEncoder commandEncoder);
        ~WebGPUCommandBuffer() override;

        virtual void CopyBufferToBuffer(GPUBuffer* src, GPUBuffer* dest, uint64 size, int64 srcOffset, int64 destOffset) override final;
        virtual void CopyTextureToTexture(
            GPUTexture* src, GPUTexture* dest,
            TextureSubresource srcSubresource, Offset3D srcOffset,
            TextureSubresource destSubresource, Offset3D destOffset,
            Extent3D extent
        ) override final;
        virtual void CopyBufferToTexture(
            GPUBuffer* src, GPUTexture* dest,
            int64 bufferOffset, uint64 rowPitch, uint64 rowCount,
            TextureSubresource subresource, Offset3D textureOffset, Extent3D textureExtent
        ) override final;
        virtual void CopyTextureToBuffer(
            GPUTexture* src, GPUBuffer* dest,
            int64 bufferOffset, uint64 rowPitch, uint64 rowCount,
            TextureSubresource subresource, Offset3D textureOffset, Extent3D textureExtent
        ) override final;

        virtual void SetPipeline(GPUPipeline* pipeline) override final;
        virtual void SetDepthStencilState(DepthStencilState* depthStencilState) override final;
        virtual void SetBlendState(BlendState* blendState) override final;

        virtual void BeginRenderPass(RenderPassDesc* renderPassDesc) override final;
        virtual void EndRenderPass() override final;

        virtual void Dispatch(uint32 sizeX, uint32 sizeY, uint32 sizeZ) override final;
        virtual void DispatchIndirect(GPUBuffer* dispatchCommands, int64 offset) override final;

        virtual void SetViewport(Viewport* viewport) override final;
        virtual void SetScissor(Rect2D* scissor) override final;
        virtual void DrawIndexed(uint32 indexCount, uint32 instanceCount, uint32 firstIndex, int32 vertexOffset, uint32 firstInstance) override final;
        virtual void DrawIndexedIndirect(GPUBuffer* drawCommands, int64 offset) override final;

        virtual BackendAPI GetBackendAPI() const override final { return BackendAPI_WebGPU; }

        /// @brief Complete command recording and return the resulting command buffer.
        /// @return 
        WGPUCommandBuffer Finish() const;

    private:
        WGPUCommandEncoder _commandEncoder = nullptr;
        WGPURenderPassEncoder _activeRenderPass = nullptr; //< The active render pass is transient, i.e. not necessarily initialized.
        WGPUComputePassEncoder _activeComputePass = nullptr; //< The active compute pass is transient, i.e. not necessarily initialized.
    };

    /// @brief WebGPU graphics device implementation.
    class WebGPUGraphicsDevice : public IGraphicsDevice
    {
    private:
        WebGPUGraphicsDevice(WGPUInstance instance, WGPUAdapter adapter, WGPUDevice device);

    public:
        ~WebGPUGraphicsDevice();

        /// @brief Try to create a WebGPU graphics device based on the provided device description.
        /// @param graphicsDeviceDesc Device description to use.
        /// @return A new WebGPUGraphicsDevice, or nullptr on failure.
        static IGraphicsDevice* TryCreate(GraphicsDeviceDesc const& graphicsDeviceDesc);

        [[nodiscard]] virtual GPUBuffer* CreateBuffer(GPUBufferDesc* bufferDesc) override final;
        virtual void DestroyBuffer(GPUBuffer* buffer) override final;

        [[nodiscard]] virtual GPUTexture* CreateTexture(GPUTextureDesc* textureDesc) override final;
        virtual GPUTexture* CreateTextureView(GPUTextureViewDesc* textureViewDesc) override final;
        virtual void DestroyTexture(GPUTexture* texture) override final;

        virtual GPUSampler* CreateSampler(GPUSamplerDesc* samplerDesc) override final;
        virtual void DestroySampler(GPUSampler* sampler) override final;

        [[nodiscard]] virtual GPUPipeline* CreateGraphicsPipeline(GPUGraphicsPipelineDesc* pipelineDesc) override final;
        [[nodiscard]] virtual GPUPipeline* CreateComputePipeline(GPUComputePipelineDesc* pipelineDesc) override final;
        virtual void DestroyPipeline(GPUPipeline* pipeline) override final;

        [[nodiscard]] virtual IGPUCommandBuffer* CreateCommandBuffer() override final;
        virtual void DestroyCommandBuffer(IGPUCommandBuffer* commandBuffer) override final;

        [[nodiscard]] virtual GraphicsDeviceResult Submit(GPUQueueType queue, IGPUCommandBuffer** commandBuffers, uint32 commandBufferCount) override final;
        
        virtual void WaitIdle() override final;

    private:
        WGPUInstance _instance = nullptr;
        WGPUAdapter _adapter = nullptr;
        WGPUDevice _device = nullptr;
        WGPUQueue _queue = nullptr;
    };
} // namespace RHI

#endif //BONSAI_RHI_WEBGPU_SUPPORT
#endif //WEBGPU_GRAPHICS_DEVICE_HPP
