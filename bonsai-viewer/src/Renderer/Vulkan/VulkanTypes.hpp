#pragma once
#ifndef VULKAN_TYPES_HPP
#define VULKAN_TYPES_HPP

#include <volk.h>
#include <vk_mem_alloc.h>

struct VulkanBuffer
{
    VkBuffer Buffer;
    VmaAllocation Allocation;
};

#endif //VULKAN_TYPES_HPP
