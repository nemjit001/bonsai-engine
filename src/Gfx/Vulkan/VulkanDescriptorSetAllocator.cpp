#include "VulkanDescriptorSetAllocator.hpp"

#include <volk.h>
#include <Core/Logger.hpp>
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
        _poolSizeInfo(poolSizeInfo),
        _activePool(0)
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
        // Attempt to allocate from all previously active pools.
        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
        for (size_t i = 0; i < _activePool + 1; i++)
        {
            VkDescriptorPool const activePool = _descriptorPools[i];
            if (TryAllocateDescriptorSet(activePool, setLayout->GetDescriptorSetLayout(), &descriptorSet)) {
                // Create new Vulkan descriptor set
                return new VulkanDescriptorSet(descriptorSet);
            }
        }

        // Create new descriptor pool if needed
        if (_activePool + 1 >= _descriptorPools.size()) {
            VkDescriptorPool descriptorPool = CreateNewDescriptorPool(_device, _poolSizeInfo);
            if (descriptorPool == VK_NULL_HANDLE) {
                SPDLOG_WARN("Failed to create a new descriptor pool");
                return nullptr;
            }

            _descriptorPools.push_back(descriptorPool);
        }

        // Bump active pool index for next allocation attempt
        _activePool++;
        return nullptr;
    }

    void VulkanDescriptorSetAllocator::Reset()
    {
        for (auto const& pool : _descriptorPools) {
            vkResetDescriptorPool(_device, pool, 0);
        }

        _activePool = 0;
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
        descriptorPoolCreateInfo.flags = 0;
        descriptorPoolCreateInfo.maxSets = poolSizeInfo.descriptorSetCount;
        descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(std::size(poolSizes));
        descriptorPoolCreateInfo.pPoolSizes = poolSizes;

        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        if (VK_CHECK_FAILED(vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &descriptorPool))) {
            return VK_NULL_HANDLE;
        }

        return descriptorPool;
    }

    bool VulkanDescriptorSetAllocator::TryAllocateDescriptorSet(VkDescriptorPool pool, VkDescriptorSetLayout setLayout, VkDescriptorSet* outDescriptorSet)
    {
        // Attempt to allocate a new descriptor set
        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
        descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.pNext = nullptr;
        descriptorSetAllocateInfo.descriptorPool = pool;
        descriptorSetAllocateInfo.descriptorSetCount = 1;
        descriptorSetAllocateInfo.pSetLayouts = &setLayout;
        
        return VK_CHECK_SUCCEEDED(vkAllocateDescriptorSets(_device, &descriptorSetAllocateInfo, outDescriptorSet));
    }
} // namespace Bonsai
