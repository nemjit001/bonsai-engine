#include "VulkanDescriptorSetAllocator.hpp"

#include <volk.h>
#include "Core/Logger.hpp"
#include "Gfx/Vulkan/VulkanGraphicsHelpers.hpp"
#include "Gfx/Vulkan/VulkanGraphicsTypes.hpp"

namespace Bonsai
{
    VulkanDescriptorSetAllocator::VulkanDescriptorSetAllocator(
        VkDevice device,
        VulkanDescriptorPoolSizeInfo const& poolSizeInfo,
        VkDescriptorPool firstDescriptorPool
    )
        :
        _device(device),
        _poolSizeInfo(poolSizeInfo)
    {
        _descriptorPools.push_back(firstDescriptorPool);
    }

    VulkanDescriptorSetAllocator::~VulkanDescriptorSetAllocator()
    {
        //
    }

    VulkanDescriptorSetAllocator* VulkanDescriptorSetAllocator::TryCreate(VkDevice device, VulkanDescriptorPoolSizeInfo const& poolSizeInfo)
    {
        // Validate descriptor pool size
        if (poolSizeInfo.descriptorSetCount == 0)
        {
            SPDLOG_ERROR("Requested pool sizes descriptor set count is 0");
            return nullptr;
        }

        // Create first descriptor pool
        VkDescriptorPool firstDescriptorPool = CreateNewDescriptorPool(device, poolSizeInfo);
        if (firstDescriptorPool == VK_NULL_HANDLE)
        {
            SPDLOG_ERROR("Failed to create first descriptor pool");
            return nullptr;
        }

        return new VulkanDescriptorSetAllocator(device, poolSizeInfo, firstDescriptorPool);
    }

    VulkanDescriptorSet* VulkanDescriptorSetAllocator::AllocateDescriptorSet(VulkanDescriptorSetLayout const* setLayout)
    {
        // Get active descriptor pool
        VkDescriptorPool activePool = _descriptorPools.back();

        // Attempt to allocate a new descriptor set
        VkDescriptorSetLayout setLayoutHandle = setLayout->GetDescriptorSetLayout();

        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
        descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.pNext = nullptr;
        descriptorSetAllocateInfo.descriptorPool = activePool;
        descriptorSetAllocateInfo.descriptorSetCount = 1;
        descriptorSetAllocateInfo.pSetLayouts = &setLayoutHandle;
        
        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
        if (VK_CHECK_FAILED(vkAllocateDescriptorSets(_device, &descriptorSetAllocateInfo, &descriptorSet)))
        {
            // Create new descriptor pool
            VkDescriptorPool newDescriptorPool = CreateNewDescriptorPool(_device, _poolSizeInfo);
            if (newDescriptorPool == VK_NULL_HANDLE)
            {
                SPDLOG_ERROR("Failed to create new descriptor pool");
                return nullptr;
            }

            // Store in descriptor pool storage
            _descriptorPools.push_back(newDescriptorPool);

            // Update allocate info and reattempt allocation
            activePool = _descriptorPools.back();
            descriptorSetAllocateInfo.descriptorPool = activePool;
            if (VK_CHECK_FAILED(vkAllocateDescriptorSets(_device, &descriptorSetAllocateInfo, &descriptorSet))) {
                return nullptr;
            }
        }

        // Create new Vulkan descriptor set
        return new VulkanDescriptorSet(_device, activePool, descriptorSet);
    }

    VkDescriptorPool VulkanDescriptorSetAllocator::CreateNewDescriptorPool(VkDevice device, VulkanDescriptorPoolSizeInfo const& poolSizeInfo)
    {
        VkDescriptorPoolSize poolSizes[] = {
            { VK_DESCRIPTOR_TYPE_SAMPLER, poolSizeInfo.descriptorSetCount * poolSizeInfo.samplerDescriptorMultiplier },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, poolSizeInfo.descriptorSetCount * poolSizeInfo.combinedImageSamplerDescriptorMultiplier },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, poolSizeInfo.descriptorSetCount * poolSizeInfo.sampledImageDescriptorMultiplier },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, poolSizeInfo.descriptorSetCount * poolSizeInfo.storageImageDescriptorMultiplier },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, poolSizeInfo.descriptorSetCount * poolSizeInfo.uniformBufferDescriptorMultiplier },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, poolSizeInfo.descriptorSetCount * poolSizeInfo.storageBufferDescriptorMultiplier },
        };

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.pNext = nullptr;
        descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT; // We will free descriptor sets
        descriptorPoolCreateInfo.maxSets = poolSizeInfo.descriptorSetCount;
        descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(std::size(poolSizes));
        descriptorPoolCreateInfo.pPoolSizes = poolSizes;

        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        if (VK_CHECK_FAILED(vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &descriptorPool))) {
            return VK_NULL_HANDLE;
        }

        return descriptorPool;
    }
} // namespace Bonsai
