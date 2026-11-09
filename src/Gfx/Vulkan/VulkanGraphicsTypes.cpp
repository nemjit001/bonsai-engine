#include "VulkanGraphicsTypes.hpp"

#include <volk.h>

namespace Bonsai
{
    VulkanBuffer::VulkanBuffer(VkDevice device, VkDeviceMemory memory, VkBuffer buffer)
        :
        _device(device),
        _memory(memory),
        _buffer(buffer)
    {
        //
    }

    VulkanBuffer::~VulkanBuffer()
    {
        vkDestroyBuffer(_device, _buffer, nullptr);
        vkFreeMemory(_device, _memory, nullptr);
    }
} // namespace Bonsai
