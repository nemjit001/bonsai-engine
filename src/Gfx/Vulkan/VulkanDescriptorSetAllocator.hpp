#pragma once
#ifndef BONSAI_VULKAN_DESCRIPTOR_POOL_HPP
#define BONSAI_VULKAN_DESCRIPTOR_POOL_HPP

#define VK_NO_PROTOTYPES

#include <vulkan/vulkan.h>

namespace Bonsai
{
    /// @brief The VulkanDescriptorSetAllocator implements descriptor set allocation from Vulkan descriptor pools,
    /// using an internal pool allocator.
    class VulkanDescriptorSetAllocator
    {
    private:
        VulkanDescriptorSetAllocator(VkDevice device);

    public:
        ~VulkanDescriptorSetAllocator();

        VulkanDescriptorSetAllocator(VulkanDescriptorSetAllocator const&) = delete;
        VulkanDescriptorSetAllocator& operator=(VulkanDescriptorSetAllocator const&) = delete;

        /// @brief Try to create a Vulkan descriptor set allocator.
        /// @param device Logical device to use in the descriptor set allocator.
        /// @return 
        static VulkanDescriptorSetAllocator* TryCreate(VkDevice device);

        /// @brief Allocate a descriptor set from the descriptor pool.
        /// @param setLayout Set layout to use.
        /// @param outDescriptorSet Output descriptor set.
        /// @return A boolean indicating success.
        bool AllocateDescriptorSet(VkDescriptorSetLayout setLayout, VkDescriptorSet* outDescriptorSet);

    private:
        VkDevice _device;
        // TODO(nemjit001): implement descriptor pool allocator
    };
} // namespace Bonsai

#endif //BONSAI_VULKAN_DESCRIPTOR_POOL_HPP
