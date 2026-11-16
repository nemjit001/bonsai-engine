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

    VulkanTextureView::VulkanTextureView(VkDevice device, VkImageView imageView)
        :
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

    VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout _descriptorSetLayout)
        :
        _device(device),
        _descriptorSetLayout(_descriptorSetLayout)
    {
        //
    }

    VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
    {
        vkDestroyDescriptorSetLayout(_device, _descriptorSetLayout, nullptr);
    }

    VulkanPipelineLayout::VulkanPipelineLayout(VkDevice device, VkPipelineLayout pipelineLayout)
        :
        _device(device),
        _pipelineLayout(pipelineLayout)
    {
        //
    }

    VulkanPipelineLayout::~VulkanPipelineLayout()
    {
        vkDestroyPipelineLayout(_device, _pipelineLayout, nullptr);
    }

    VulkanRenderPipeline::VulkanRenderPipeline(VkDevice device, VkPipeline pipeline)
        :
        _device(device),
        _pipeline(pipeline)
    {
        //
    }

    VulkanRenderPipeline::~VulkanRenderPipeline()
    {
        vkDestroyPipeline(_device, _pipeline, nullptr);
    }

    VulkanComputePipeline::VulkanComputePipeline(VkDevice device, VkPipeline pipeline)
        :
        _device(device),
        _pipeline(pipeline)
    {
        //
    }

    VulkanComputePipeline::~VulkanComputePipeline()
    {
        vkDestroyPipeline(_device, _pipeline, nullptr);
    }
} // namespace Bonsai
