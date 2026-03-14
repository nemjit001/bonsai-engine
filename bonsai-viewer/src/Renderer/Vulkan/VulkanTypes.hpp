#pragma once
#ifndef VULKAN_TYPES_HPP
#define VULKAN_TYPES_HPP

#include <volk.h>
#include <vk_mem_alloc.h>

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

#endif //VULKAN_TYPES_HPP
