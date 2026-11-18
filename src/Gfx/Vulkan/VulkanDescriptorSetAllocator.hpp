#pragma once
#ifndef BONSAI_VULKAN_DESCRIPTOR_POOL_HPP
#define BONSAI_VULKAN_DESCRIPTOR_POOL_HPP

#define VK_NO_PROTOTYPES

#include <vector>
#include <vulkan/vulkan.h>

namespace Bonsai
{
    class VulkanDescriptorSetLayout;
    class VulkanDescriptorSet;

    /// @brief The VulkanDescriptorPoolSizeInfo stores the max descriptor sets allocated from a pool,
    /// along with the expected descriptors allocated per set (multipliers).
    struct VulkanDescriptorPoolSizeInfo
    {
        uint32_t descriptorSetCount;
        uint32_t samplerDescriptorMultiplier;
        uint32_t combinedImageSamplerDescriptorMultiplier;
        uint32_t sampledImageDescriptorMultiplier;
        uint32_t storageImageDescriptorMultiplier;
        uint32_t uniformBufferDescriptorMultiplier;
        uint32_t storageBufferDescriptorMultiplier;
    };

    /// @brief The VulkanDescriptorSetAllocator implements descriptor set allocation from Vulkan descriptor pools,
    /// using an internal pool allocator.
    class VulkanDescriptorSetAllocator
    {
    private:
        VulkanDescriptorSetAllocator(
            VkDevice device,
            VulkanDescriptorPoolSizeInfo const& poolSizeInfo,
            VkDescriptorPool firstDescriptorPool
        );

    public:
        ~VulkanDescriptorSetAllocator();

        VulkanDescriptorSetAllocator(VulkanDescriptorSetAllocator const&) = delete;
        VulkanDescriptorSetAllocator& operator=(VulkanDescriptorSetAllocator const&) = delete;

        /// @brief Try to create a Vulkan descriptor set allocator.
        /// @param device Logical device to use in the descriptor set allocator.
        /// @param poolSizeInfo Pool size info to use for per-pool descriptor pool allocation.
        /// @return 
        static VulkanDescriptorSetAllocator* TryCreate(VkDevice device, VulkanDescriptorPoolSizeInfo const& poolSizeInfo);

        /// @brief Allocate a descriptor set from the descriptor pool.
        /// @param setLayout Set layout to use for allocation.
        /// @return The new descriptor set.
        VulkanDescriptorSet* AllocateDescriptorSet(VulkanDescriptorSetLayout const* setLayout);

    private:
        /// @brief Create a new descriptor pool.
        /// @param device Device to use for pool creation.
        /// @param poolSizeInfo Pool size info to use for pool creation.
        /// @return The new descriptor pool.
        static VkDescriptorPool CreateNewDescriptorPool(VkDevice device, VulkanDescriptorPoolSizeInfo const& poolSizeInfo);

    private:
        VkDevice _device;
        VulkanDescriptorPoolSizeInfo _poolSizeInfo;
        std::vector<VkDescriptorPool> _descriptorPools;
    };
} // namespace Bonsai

#endif //BONSAI_VULKAN_DESCRIPTOR_POOL_HPP
