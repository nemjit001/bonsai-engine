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
    
    VulkanTexture* VulkanObjectFactory::CreateTexture(GraphicsTextureDesc const& textureDesc)
    {
        // Set texture depth and layers based on texture type
        uint32_t const depth = textureDesc.textureType == GraphicsTextureType_3D ? textureDesc.depthOrLayers : 1;
        uint32_t const layers = textureDesc.textureType != GraphicsTextureType_3D ? textureDesc.depthOrLayers : 1;

        // Create Vulkan image
        VkImageCreateInfo imageCreateInfo{};
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.pNext = nullptr;
        imageCreateInfo.flags = 0;
        imageCreateInfo.imageType = VulkanGraphicsHelpers::GetVulkanImageType(textureDesc.textureType);
        imageCreateInfo.format = VulkanGraphicsHelpers::GetVulkanFormat(textureDesc.textureFormat);
        imageCreateInfo.extent.width = textureDesc.width;
        imageCreateInfo.extent.height = textureDesc.height;
        imageCreateInfo.extent.depth = depth;
        imageCreateInfo.arrayLayers = layers;
        imageCreateInfo.mipLevels = textureDesc.mipLevels;
        imageCreateInfo.samples = VulkanGraphicsHelpers::GetVulkanSampleCountFlagBits(textureDesc.sampleCount);
        imageCreateInfo.tiling = VulkanGraphicsHelpers::GetVulkanImageTiling(textureDesc.tilingMode);
        imageCreateInfo.usage = VulkanGraphicsHelpers::GetVulkanImageUsageFlags(textureDesc.usage);
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageCreateInfo.queueFamilyIndexCount = 0;
        imageCreateInfo.pQueueFamilyIndices = nullptr;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VkImage image = VK_NULL_HANDLE;
        if (VK_CHECK_FAILED(vkCreateImage(_device, &imageCreateInfo, nullptr, &image))) {
            return nullptr;
        }

        // Query image memory requirements
        VkMemoryRequirements memoryRequirements{};
        vkGetImageMemoryRequirements(_device, image, &memoryRequirements);

        // Try to allocate memory
        std::optional<VkDeviceMemory> memory = TryAllocateMemory(textureDesc.heapType, memoryRequirements);
        if (!memory.has_value())
        {
            vkDestroyImage(_device, image, nullptr);
            return nullptr;
        }

        // Bind image memory
        if (VK_CHECK_FAILED(vkBindImageMemory(_device, image, memory.value(), 0)))
        {
            vkDestroyImage(_device, image, nullptr);
            vkFreeMemory(_device, memory.value(), nullptr);
            return nullptr;
        }

        return new VulkanTexture(_device, memory.value(), image);
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
