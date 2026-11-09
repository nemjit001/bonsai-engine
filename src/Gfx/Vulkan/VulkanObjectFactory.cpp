#include "VulkanObjectFactory.hpp"

#include <cassert>
#include <volk.h>
#include "Gfx/Vulkan/VulkanGraphicsHelpers.hpp"
#include "Gfx/Vulkan/VulkanSwapchain.hpp"

namespace Bonsai
{
    VulkanObjectFactory::VulkanObjectFactory(
        VkInstance instance,
        VkPhysicalDevice physicalDevice,
        VkDevice device,
        VulkanSharedDeviceState* sharedDeviceState
    )
        :
        _instance(instance),
        _physicalDevice(physicalDevice),
        _device(device),
        _sharedDeviceState(sharedDeviceState)
    {
        //
    }

    VulkanObjectFactory::~VulkanObjectFactory()
    {
        //
    }

    VulkanObjectFactory* VulkanObjectFactory::TryCreate(
        VkInstance instance,
        VkPhysicalDevice physicalDevice,
        VkDevice device,
        VulkanSharedDeviceState* sharedDeviceState
    )
    {
        assert(instance != VK_NULL_HANDLE && "Vulkan instance was VK_NULL_HANDLE!");
        assert(physicalDevice != VK_NULL_HANDLE && "Vulkan physical device was VK_NULL_HANDLE!");
        assert(device != VK_NULL_HANDLE && "Vulkan device was VK_NULL_HANDLE!");
        assert(sharedDeviceState != nullptr && "Shared device state was null!");

        return new VulkanObjectFactory(instance, physicalDevice, device, sharedDeviceState);
    }
    
    VulkanBuffer* VulkanObjectFactory::CreateBuffer(GraphicsBufferDesc const& bufferDesc)
    {
        // Create vulkan buffer
        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.pNext = nullptr;
        bufferCreateInfo.flags = 0;
        bufferCreateInfo.size = static_cast<uint32_t>(bufferDesc.size);
        bufferCreateInfo.usage = VulkanGraphicsHelpers::GetVulkanBufferUsageFlags(bufferDesc.usage);
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferCreateInfo.queueFamilyIndexCount = 0;
        bufferCreateInfo.pQueueFamilyIndices = nullptr;

        VkBuffer buffer = VK_NULL_HANDLE;
        if (VK_CHECK_FAILED(vkCreateBuffer(_device, &bufferCreateInfo, nullptr, &buffer))) {
            return nullptr;
        }

        // Query buffer memory requirements
        VkMemoryRequirements memoryRequirements{};
        vkGetBufferMemoryRequirements(_device, buffer, &memoryRequirements);

        // Try to allocate memory
        std::optional<VkDeviceMemory> memory = TryAllocateMemory(bufferDesc.heapType, memoryRequirements);
        if (!memory.has_value())
        {
            vkDestroyBuffer(_device, buffer, nullptr);
            return nullptr;
        }

        // Bind buffer memory
        if (VK_CHECK_FAILED(vkBindBufferMemory(_device, buffer, memory.value(), 0)))
        {
            vkDestroyBuffer(_device, buffer, nullptr);
            vkFreeMemory(_device, memory.value(), nullptr);
            return nullptr;
        }

        // Create buffer
        return new VulkanBuffer(_device, memory.value(), buffer);
    }
    
    VulkanSwapchain* VulkanObjectFactory::CreateSwapchain(GraphicsSwapchainDesc const& swapchainDesc)
    {
        // Create swapchain
        VulkanSwapchain* swapchain = VulkanSwapchain::TryCreate(swapchainDesc.window, _instance, _physicalDevice, _device, _sharedDeviceState);
        if (swapchain == nullptr) {
            return nullptr;
        }
        
        // Configure swapchain
        VulkanSwapchainDesc vulkanSwapchainDesc{};
        vulkanSwapchainDesc.width = swapchainDesc.width;
        vulkanSwapchainDesc.height = swapchainDesc.height;
        vulkanSwapchainDesc.syncInterval = swapchainDesc.syncInterval;
        vulkanSwapchainDesc.allowTearing = swapchainDesc.allowTearing;

        if (!swapchain->Configure(vulkanSwapchainDesc))
        {
            swapchain->ReleaseRef();
            return nullptr;
        }

        return swapchain;
    }

    std::optional<VkDeviceMemory> VulkanObjectFactory::TryAllocateMemory(GraphicsHeapType heapType, VkMemoryRequirements const& memoryRequirements)
    {
        // Get required memory property flags
        VkMemoryPropertyFlags const requiredMemoryProperties = VulkanGraphicsHelpers::GetVulkanMemoryPropertyFlags(heapType);
        if (requiredMemoryProperties == 0) {
            return {};
        }

        // Get memory type index
        uint32_t const memoryTypeIndex = VulkanGraphicsHelpers::FindMemoryTypeIndex(
            _sharedDeviceState->physicalDeviceMemoryProperties2.memoryProperties,
            memoryRequirements.memoryTypeBits,
            requiredMemoryProperties
        );
        if (memoryTypeIndex == UINT32_MAX) {
            return {};
        }

        // Allocate device memory
        VkMemoryAllocateInfo memoryAllocateInfo{};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.pNext = nullptr;
        memoryAllocateInfo.allocationSize = memoryRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

        VkDeviceMemory memory = VK_NULL_HANDLE;
        if (VK_CHECK_FAILED(vkAllocateMemory(_device, &memoryAllocateInfo, nullptr, &memory))) {
            return {};
        }

        return memory;
    }
} // namespace Bonsai
