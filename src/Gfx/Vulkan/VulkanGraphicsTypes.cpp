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

    VulkanTexture::VulkanTexture(VkDevice device, VkDeviceMemory memory, VkImage image)
        :
        _device(device),
        _memory(memory),
        _image(image)
    {
        //
    }

    VulkanTexture::~VulkanTexture()
    {
        vkDestroyImage(_device, _image, nullptr);
        vkFreeMemory(_device, _memory, nullptr);
    }
} // namespace Bonsai
