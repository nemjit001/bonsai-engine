#pragma once
#ifndef VULKAN_RENDERER_HPP
#define VULKAN_RENDERER_HPP
#if BONSAI_VULKAN_SUPPORT

#include <memory>
#include <vector>
#include <volk.h>
#include <vk_mem_alloc.h>
#include "RenderBackend.hpp"

/// @brief Command list implementation.
struct BVulkanCommandListImpl;

/// @brief Renderer backend implementation.
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

/// @brief Vulkan implementation of a GPU command list.
class BVulkanCommandList : public IGPUCommandList
{
public:
    BVulkanCommandList(BVulkanRenderBackendImpl const* RendererImpl);
    ~BVulkanCommandList() override;

    bool Reset() override;

    bool Close() override;

    void SetPipelineState(BGPUPipelineStateHandle PipelineState) override;

    void Dispatch(uint32_t X, uint32_t Y, uint32_t Z) override;

    /// @brief Get the underlying implementation pointer. Command lists are available as full types in the API,
    /// so this function may be used by the render backend to query backend-specific data.
    /// @return 
    BVulkanCommandListImpl const* GetImpl() const { return pImpl.get(); }

private:
    std::unique_ptr<BVulkanCommandListImpl> pImpl;
};

/// @brief Vulkan implementation of a renderer backend.
class BVulkanRenderBackend : public IRenderBackend
{
public:
    BVulkanRenderBackend();
    ~BVulkanRenderBackend() override;

    RendererResult Init() override;

    void Shutdown() override;

    void NewFrame() override;

    void Present() override;

    uint64_t GetFrameIndex() override;

    void Execute(GPUCommandQueue CommandQueue, uint32_t CommandListCount, IGPUCommandList** CommandLists) override;

    IGPUCommandList* CreateCommandList() override;

    void DestroyCommandList(IGPUCommandList* CommandList) override;

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
#endif //VULKAN_RENDERER_HPP
