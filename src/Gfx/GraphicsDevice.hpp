#pragma once
#ifndef BONSAI_GRAPHICS_DEVICE_HPP
#define BONSAI_GRAPHICS_DEVICE_HPP

#include "Gfx/GraphicsCommandBuffer.hpp"
#include "Gfx/GraphicsTypes.hpp"

namespace Bonsai
{
    /// @brief The GraphicsDevice provides an interface to low level graphics APIs.
    class GraphicsDevice
    {
    public:
        GraphicsDevice() = default;
        virtual ~GraphicsDevice() = default;
        
        GraphicsDevice(GraphicsDevice const&) = delete;
        GraphicsDevice& operator=(GraphicsDevice const&) = delete;

        /// @brief Try to create a graphics device.
        /// @return The newly created graphics device, or nullptr on failure.
        [[nodiscard]] static GraphicsDevice* TryCreate();

        /// @brief Destroy a graphics device.
        /// @param graphicsDevice Graphics device to destroy.
        static void Destroy(GraphicsDevice* graphicsDevice);

        /// @brief Create a buffer using this graphics device.
        /// @param bufferDesc Buffer description.
        /// @return A new graphics buffer, or nullptr on failure.
        [[nodiscard]] virtual GraphicsBuffer* CreateBuffer(GraphicsBufferDesc const& bufferDesc) = 0;

        /// @brief Create a texture using this graphics device.
        /// @param textureDesc Texture description.
        /// @return A new graphics texture, or nullptr on failure.
        [[nodiscard]] virtual GraphicsTexture* CreateTexture(GraphicsTextureDesc const& textureDesc) = 0;

        /// @brief Create a texture view using this graphics device.
        /// @param textureViewDesc Texture view description.
        /// @return A new graphics texture view, or nullptr on failure.
        [[nodiscard]] virtual GraphicsTextureView* CreateTextureView(GraphicsTextureViewDesc const& textureViewDesc) = 0;

        /// @brief Create a sampler using this graphics device.
        /// @param samplerDesc Sampler description.
        /// @return A new graphics sampler, or nullptr on failure.
        [[nodiscard]] virtual GraphicsSampler* CreateSampler(GraphicsSamplerDesc const& samplerDesc) = 0;

        /// @brief Create a descriptor set layout using this graphics device.
        /// @param descriptorSetLayoutDesc Descriptor set layout description.
        /// @return A new descriptor set layout, or nullptr on failure.
        [[nodiscard]] virtual GraphicsDescriptorSetLayout* CreateDescriptorSetLayout(GraphicsDescriptorSetLayoutDesc const& descriptorSetLayoutDesc) = 0;
 
        /// @brief Create a descriptor set using this graphics device. Descriptor sets are invalidated after frame completion.
        /// @param frameContext Frame context to allocate a descriptor set for.
        /// @param descriptorSetDesc Descriptor set description.
        /// @return A new descriptor set, or nullptr on failure.
        [[nodiscard]] virtual GraphicsDescriptorSet* CreateDescriptorSet(FrameContext* frameContext, GraphicsDescriptorSetDesc const& descriptorSetDesc) = 0;

        /// @brief Create a pipeline layout using this graphics device.
        /// @param pipelineLayoutDesc Pipeline description.
        /// @return A new graphics pipeline layout, or nullptr on failure.
        [[nodiscard]] virtual GraphicsPipelineLayout* CreatePipelineLayout(GraphicsPipelineLayoutDesc const& pipelineLayoutDesc) = 0;

        /// @brief Create a render pipeline using this graphics device.
        /// @param renderPipelineDesc Render pipeline description.
        /// @return A new graphics render pipeline, or nullptr on failure.
        [[nodiscard]] virtual GraphicsRenderPipeline* CreateRenderPipeline(GraphicsRenderPipelineDesc const& renderPipelineDesc) = 0;

        /// @brief Create a compute pipeline using this graphics device.
        /// @param computePipelineDesc Compute pipeline description.
        /// @return A new graphics compute pipeline, or nullptr on failure.
        [[nodiscard]] virtual GraphicsComputePipeline* CreateComputePipeline(GraphicsComputePipelineDesc const& computePipelineDesc) = 0;

        /// @brief Create a swapchain using this graphics device.
        /// @param swapchainDesc Swapchain description.
        /// @return A new graphics swapchain, or nullptr on failure.
        [[nodiscard]] virtual GraphicsSwapchain* CreateSwapchain(GraphicsSwapchainDesc const& swapchainDesc) = 0;

        /// @brief Destroy a buffer.
        /// @param buffer Buffer to destroy.
        virtual void DestroyBuffer(GraphicsBuffer* buffer) = 0;

        /// @brief Destroy a texture.
        /// @param texture Texture to destroy.
        virtual void DestroyTexture(GraphicsTexture* texture) = 0;

        /// @brief Destroy a texture view.
        /// @param textureView Texture view to destroy.
        virtual void DestroyTextureView(GraphicsTextureView* textureView) = 0;

        /// @brief Destroy a sampler.
        /// @param sampler Sampler to destroy.
        virtual void DestroySampler(GraphicsSampler* sampler) = 0;

        /// @brief Destroy a descriptor set layout.
        /// @param descriptorSetLayout Descriptor set layout to destroy.
        virtual void DestroyDescriptorSetLayout(GraphicsDescriptorSetLayout* descriptorSetLayout) = 0;

        /// @brief Destroy a descriptor set.
        /// @param descriptorSet Descriptor set to destroy.
        virtual void DestroyDescriptorSet(GraphicsDescriptorSet* descriptorSet) = 0;

        /// @brief Destroy a pipeline layout.
        /// @param pipelineLayout Pipeline layout to destroy.
        virtual void DestroyPipelineLayout(GraphicsPipelineLayout* pipelineLayout) = 0;

        /// @brief Destroy a render pipeline.
        /// @param renderPipeline Render pipeline to destroy.
        virtual void DestroyRenderPipeline(GraphicsRenderPipeline* renderPipeline) = 0;

        /// @brief Destroy a compute pipeline.
        /// @param computePipeline Compute pipeline to destroy.
        virtual void DestroyComputePipeline(GraphicsComputePipeline* computePipeline) = 0;

        /// @brief Destroy a swapchain.
        /// @param swapchain Swapchain to destroy.
        virtual void DestroySwapchain(GraphicsSwapchain* swapchain) = 0;

        /// @brief Copy data to a target buffer.
        /// @param buffer Buffer to copy data to.
        /// @param data Pointer to the data to copy.
        /// @param size Size of the copy region.
        /// @param offset Offset into the target buffer.
        virtual void CopyToBuffer(GraphicsBuffer* buffer, void const* data, size_t size, size_t offset) = 0;

        /// @brief Copy data from a target buffer.
        /// @param buffer Buffer to copy data from.
        /// @param outData Output data pointer to copy data to.
        /// @param size Size of the copy region.
        /// @param offset Offset into the target buffer.
        virtual void CopyFromBuffer(GraphicsBuffer* buffer, void* outData, size_t size, size_t offset) = 0;

        /// @brief Begin a command buffer for recording rendering commands.
        /// @return The command buffer.
        [[nodiscard]] virtual GraphicsCommandBuffer* BeginCommandBuffer() = 0;

        /// @brief Start a new frame on the graphics device.
        /// @param swapchain Swapchains participating in this frame.
        /// @param swapchainCount Number of swapchains in the swapchains array.
        /// @return The frame context for the started frame.
        [[nodiscard]] virtual FrameContext* StartFrame(GraphicsSwapchain** swapchains, size_t swapchainCount) = 0;

        /// @brief Submit commands to a frame context.
        /// @param frameContext Frame context to submit commands to.
        /// @param commandBuffer Command buffer to submit.
        virtual void SubmitFrameCommands(FrameContext* frameContext, GraphicsCommandBuffer* commandBuffer) = 0;

        /// @brief Submit a frame to the graphics device.
        /// @param frameContext Frame context to submit.
        virtual void SubmitFrame(FrameContext* frameContext) = 0;

        /// @brief Start a new transfer batch on the graphics device.
        /// @return The new transfer batch.
        [[nodiscard]] virtual TransferBatch* StartTransferBatch() = 0;

        /// @brief Submit commands to a transfer batch.
        /// @param transferBatch Transfer batch to submit commands to.
        /// @param commandBuffer Command buffer to submit.
        virtual void SubmitTransferCommands(TransferBatch* transferBatch, GraphicsCommandBuffer* commandBuffer) = 0;

        /// @brief Submit a transfer batch to the graphics device.
        /// @param transferBatch Transfer batch to submit.
        virtual void SubmitTransferBatch(TransferBatch* transferBatch) = 0;

        /// @brief Wait for a transfer batch to complete.
        /// @param transferBatch Transfer batch to wait for.
        virtual void WaitForTransferBatch(TransferBatch* transferBatch) = 0;

        /// @brief Get the active backbuffer texture for a swapchain.
        /// @param swapchain Swapchain to query. MUST be participating in the active frame.
        /// @return The graphics texture for the swapchain backbuffer.
        virtual GraphicsTexture* GetSwapchainBackbuffer(GraphicsSwapchain* swapchain) = 0;

        /// @brief Get the active backbuffer texture view for a swapchain.
        /// @param swapchain Swapchain to query. MUST be participating in the active frame.
        /// @return The graphics texture view for the swapchain backbuffer.
        virtual GraphicsTextureView* GetSwapchainBackbufferView(GraphicsSwapchain* swapchain) = 0;

        /// @brief Get the extent for a swapchain.
        /// @param swapchain Swapchain to query extent for.
        /// @return The swapchain extent.
        virtual Extent2D GetSwapchainExtent(GraphicsSwapchain* swapchain) = 0;

        /// @brief Resize a swapchain.
        /// @param swapchain Swapchain to resize.
        /// @param width New swapchain width.
        /// @param height New swapchain height.
        virtual void ResizeSwapchain(GraphicsSwapchain* swapchain, uint32_t width, uint32_t height) = 0;

        /// @brief Wait for the graphics device to be idle.
        virtual void WaitForIdle() = 0;
    }; 
} // namespace Bonsai

#endif //BONSAI_GRAPHICS_DEVICE_HPP
