#pragma once
#ifndef VULKAN_RENDER_BACKEND_HPP
#define VULKAN_RENDER_BACKEND_HPP
#if BONSAI_VULKAN_SUPPORT

#include <memory>
#include <vector>
#include <volk.h>
#include <vk_mem_alloc.h>
#include "RenderBackend.hpp"

struct BVulkanRenderBackendImpl;

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
    BGPUPipelineLocalSize LocalSize;
    std::vector<VkDescriptorSetLayout> DescriptorSetLayouts;
};

/// @brief Vulkan implementation of a renderer backend.
class BVulkanRenderBackend : public IRenderBackend
{
public:
    BVulkanRenderBackend();
    ~BVulkanRenderBackend() override;

    RendererResult Init(BRenderBackendInitInfo InitInfo) override;

    void Shutdown() override;

    uint64_t GetFrameIndex() override;

    BGPUBufferHandle CreateBuffer(GPUHeapType HeapType, BGPUBufferDesc BufferDesc) override;
    
    void DestroyBuffer(BGPUBufferHandle Buffer) override;

    BGPUTextureHandle CreateTexture(GPUHeapType HeapType, BGPUTextureDesc TextureDesc) override;

    void DestroyTexture(BGPUTextureHandle Texture) override;

    BGPUPipelineStateHandle CreateComputePipelineState(BGPUComputePipelineStateDesc PipelineDesc) override;

    BGPUPipelineStateHandle CreateRenderPipelineState(BGPURenderPipelineStateDesc PipelineDesc) override;

    void DestroyPipelineState(BGPUPipelineStateHandle PipelineState) override;

    BGPUPipelineLocalSize GetPipelineLocalSize(BGPUPipelineStateHandle PipelineState) override;

private:
    std::unique_ptr<BVulkanRenderBackendImpl> pImpl;
};

#endif //BONSAI_VULKAN_SUPPORT
#endif //VULKAN_RENDER_BACKEND_HPP
