#pragma once
#ifndef BONSAI_VULKAN_GRAPHICS_TYPES_HPP
#define BONSAI_VULKAN_GRAPHICS_TYPES_HPP

#define VK_NO_PROTOTYPES

#include <vector>
#include <vulkan/vulkan.h>
#include "Gfx/Internal/GraphicsTypesInternal.hpp"

namespace Bonsai
{
    class VulkanSwapchain;

    /// @brief Vulkan buffer implementation.
    class VulkanBuffer : public GraphicsBuffer
    {
    public:
        VulkanBuffer(VkDevice device, VkDeviceMemory memory, VkBuffer buffer);

    protected:
        ~VulkanBuffer() override;

    public:
        /// @brief Get the Vulkan buffer handle.
        /// @return The Vulkan buffer.
        VkBuffer GetBuffer() const { return _buffer; }

    private:
        VkDevice _device;
        VkDeviceMemory _memory;
        VkBuffer _buffer;
    };

    /// @brief Vulkan texture implementation.
    class VulkanTexture : public GraphicsTexture
    {
    public:
        VulkanTexture(VkDevice device, VkDeviceMemory memory, VkImage image);

    protected:
        ~VulkanTexture() override;

    public:
        /// @brief Get the Vulkan image handle.
        /// @return The Vulkan image.
        VkImage GetImage() const { return _image; }

        /// @brief Set the underlying image handle. This is useful for externally managed images.
        /// Only use this if you know what you are doing.
        /// @param image Image handle to set.
        void SetImage(VkImage image) { _image = image; }

    private:
        VkDevice _device;
        VkDeviceMemory _memory;
        VkImage _image;
    };

    /// @brief Vulkan texture view implementation.
    class VulkanTextureView : public GraphicsTextureView
    {
    public:
        VulkanTextureView(VulkanTexture* parentTexture, VkDevice device, VkImageView imageView);

    protected:
        ~VulkanTextureView() override;
    
    public:
        /// @brief Get the Vulkan image view handle.
        /// @return The Vulkan image view.
        VkImageView GetImageView() const { return _imageView; }

        /// @brief Set the underlying image view handle. This is useful for externally managed image views.
        /// Only use this if you know what you are doing.
        /// @param imageView Image view handle to set.
        void SetImageView(VkImageView imageView) { _imageView = imageView; }

    private:
        VkDevice _device;
        VkImageView _imageView;
    };

    /// @brief Vulkan sampler implementation.
    class VulkanSampler : public GraphicsSampler
    {
    public:
        VulkanSampler(VkDevice device, VkSampler sampler);

    protected:
        ~VulkanSampler() override;

    private:
        VkDevice _device;
        VkSampler _sampler;
    };

    /// @brief Shared state for Vulkan devices.
    struct VulkanSharedDeviceState
    {
        uint64_t framesInFlight;
        VkPhysicalDeviceMemoryProperties2 physicalDeviceMemoryProperties2;
        uint32_t graphicsQueueFamily;
        uint32_t presentQueueFamily;
        uint32_t computeQueueFamily;
        uint32_t transferQueueFamily;
        VkQueue graphicsQueue;
        VkQueue presentQueue;
        VkQueue computeQueue;
        VkQueue transferQueue;
    };

    /// @brief Vulkan swapchain info for tracking active swap chains for a frame.
    struct VulkanFrameSwapchainInfo
    {
        std::vector<VkSwapchainKHR> swapchains;
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkPipelineStageFlags> dstWaitStageMasks;
        std::vector<uint32_t> imageIndices;
    };
    
    /// @brief Vulkan frame context implementation. Exposes Vulkan frame state.
    class VulkanFrameContext : public FrameContext
    {
    public:
        VkFence frameReadyFence;
        VkSemaphore renderingFinishedSemaphore;
        VkCommandPool graphicsCommandPool;
        VkCommandBuffer frameCommandBuffer;
        std::vector<VulkanSwapchain*> participatingSwapchains;
        VulkanFrameSwapchainInfo swapchainInfo;
    };

    /// @brief Vulkan transfer batch implementation, Exposes Vulkan transfer batch state.
    class VulkanTransferBatch : public TransferBatch
    {
    public:
        uint32_t batchIndex;
        VkFence transferFinishedFence;
        VkCommandPool transferCommandPool;
        VkCommandBuffer transferCommandBuffer;
    };
} // namespace Bonsai

#endif //BONSAI_VULKAN_GRAPHICS_TYPES_HPP
