#pragma once
#ifndef VULKAN_RENDERER_HPP
#define VULKAN_RENDERER_HPP
#if BONSAI_VULKAN_SUPPORT

#include <memory>
#include <volk.h>
#include <vk_mem_alloc.h>
#include "Renderer.hpp"

/// @brief Vulkan GPU buffer implementation.
class BGPUBuffer
{
public:
    VkBuffer Buffer;
    VmaAllocation Allocation;
};

/// @brief Vulkan GPU texture implementation.
class BGPUTexture
{
public:
    VkImage Image;
    VmaAllocation Allocation;
};

/// @brief Vulkan GPU pipeline state implementation.
class BGPUPipelineState
{
public:
    VkPipelineBindPoint BindPoint;
    VkPipelineLayout PipelineLayout;
    VkPipeline Pipeline;
};

/// @brief Renderer backend implementation.
struct BVulkanRendererImpl;

/// @brief Vulkan implementation of a renderer backend.
class BVulkanRenderer : public BRenderer
{
public:
    BVulkanRenderer();
    ~BVulkanRenderer() override;

    RendererResult Init() override;

    void Shutdown() override;

    void Frame() override;

    TRenderHandle<BGPUBuffer> CreateBuffer(GPUHeapType HeapType, BGPUBufferDesc BufferDesc) override;
    
    void DestroyBuffer(TRenderHandle<BGPUBuffer> Buffer) override;

    TRenderHandle<BGPUTexture> CreateTexture(GPUHeapType HeapType, BGPUTextureDesc TextureDesc) override;

    void DestroyTexture(TRenderHandle<BGPUTexture> Texture) override;

    TRenderHandle<BGPUPipelineState> CreateComputePipelineState(BGPUComputePipelineStateDesc PipelineDesc) override;

    TRenderHandle<BGPUPipelineState> CreateRenderPipelineState(BGPURenderPipelineStateDesc PipelineDesc) override;

    void DestroyPipelineState(TRenderHandle<BGPUPipelineState> PipelineState) override;

private:
    std::unique_ptr<BVulkanRendererImpl> pImpl;
};

#endif //BONSAI_VULKAN_SUPPORT
#endif //VULKAN_RENDERER_HPP
