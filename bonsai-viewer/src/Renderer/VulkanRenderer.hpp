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

private:
    std::unique_ptr<BVulkanRendererImpl> pImpl;
};

#endif //VULKAN_RENDERER_HPP
