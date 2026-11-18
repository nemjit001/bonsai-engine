#include "VulkanDescriptorSetAllocator.hpp"

namespace Bonsai
{
    VulkanDescriptorSetAllocator::VulkanDescriptorSetAllocator(VkDevice device)
        :
        _device(device)
    {
        //
    }

    VulkanDescriptorSetAllocator::~VulkanDescriptorSetAllocator()
    {
        //
    }

    VulkanDescriptorSetAllocator* VulkanDescriptorSetAllocator::TryCreate(VkDevice device)
    {
        return new VulkanDescriptorSetAllocator(device);
    }

    bool VulkanDescriptorSetAllocator::AllocateDescriptorSet(VkDescriptorSetLayout setLayout, VkDescriptorSet* outDescriptorSet)
    {
        return false;
    }
} // namespace Bonsai
