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

    VulkanTextureView::VulkanTextureView(VulkanTexture* parentTexture, VkDevice device, VkImageView imageView)
        :
        GraphicsTextureView(parentTexture),
        _device(device),
        _imageView(imageView)
    {
        //
    }

    VulkanTextureView::~VulkanTextureView()
    {
        vkDestroyImageView(_device, _imageView, nullptr);
    }
    
    VulkanSampler::VulkanSampler(VkDevice device, VkSampler sampler)
        :
        _device(device),
        _sampler(sampler)
    {
        //
    }

    VulkanSampler::~VulkanSampler()
    {
        vkDestroySampler(_device, _sampler, nullptr);
    }
} // namespace Bonsai
