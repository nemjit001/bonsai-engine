#pragma once
#ifndef VULKAN_RENDERER_HPP
#define VULKAN_RENDERER_HPP

#include <memory>
#include "Renderer.hpp"

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

private:
    std::unique_ptr<BVulkanRendererImpl> pImpl;
};

#endif //VULKAN_RENDERER_HPP
