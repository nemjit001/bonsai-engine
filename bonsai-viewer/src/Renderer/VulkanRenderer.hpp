#pragma once
#ifndef VULKAN_RENDERER_HPP
#define VULKAN_RENDERER_HPP
#if BONSAI_VULKAN_SUPPORT

#include <memory>
#include <vector>
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
    std::vector<VkDescriptorSetLayout> DescriptorSetLayouts;
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

    BGPUBufferHandle CreateBuffer(GPUHeapType HeapType, BGPUBufferDesc BufferDesc) override;
    
    void DestroyBuffer(BGPUBufferHandle Buffer) override;

    BGPUTextureHandle CreateTexture(GPUHeapType HeapType, BGPUTextureDesc TextureDesc) override;

    void DestroyTexture(BGPUTextureHandle Texture) override;

    BGPUPipelineStateHandle CreateComputePipelineState(BGPUComputePipelineStateDesc PipelineDesc) override;

    BGPUPipelineStateHandle CreateRenderPipelineState(BGPURenderPipelineStateDesc PipelineDesc) override;

    void DestroyPipelineState(BGPUPipelineStateHandle PipelineState) override;

private:
    std::unique_ptr<BVulkanRendererImpl> pImpl;
};

#endif //BONSAI_VULKAN_SUPPORT
#endif //VULKAN_RENDERER_HPP
