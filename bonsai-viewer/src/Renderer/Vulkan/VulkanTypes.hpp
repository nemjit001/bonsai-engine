#pragma once
#ifndef VULKAN_TYPES_HPP
#define VULKAN_TYPES_HPP

#include <volk.h>
#include <vk_mem_alloc.h>

class BGPUBuffer
{
public:
    VkBuffer Buffer;
    VmaAllocation Allocation;
};

class BGPUTexture
{
public:
    VkImage Image;
    VmaAllocation Allocation;
};

#endif //VULKAN_TYPES_HPP
