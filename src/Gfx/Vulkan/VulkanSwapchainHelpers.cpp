#include "VulkanSwapchainHelpers.hpp"

#include <algorithm>
#include <vector>
#include <volk.h>
#include "Core/Logger.hpp"

namespace Bonsai::VulkanSwapchainHelpers
{
    bool ConfigureSwapchain(VulkanSwapchainDesc const& swapchainDesc, VulkanSwapchain& swapchain)
    {
        // Set swapchain device state
        swapchain.physicalDevice = swapchainDesc.physicalDevice;
        swapchain.device = swapchainDesc.device;
        swapchain.surface = swapchainDesc.surface;
        swapchain.sharedDeviceState = swapchainDesc.sharedDeviceState;

        // Check for zero sized swapchain
        if (swapchainDesc.width == 0 || swapchainDesc.height == 0)
        {
            // Zero sized swap chains are unsupported, so we clean up swapchain data
            SPDLOG_WARN("Trying to configure a zero sized swapchain!");
            return false;
        }

        // Get surface capabilities
        VkSurfaceCapabilitiesKHR surfaceCapabilities{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(swapchain.physicalDevice, swapchain.surface, &surfaceCapabilities);

        // Query surface formats and present modes
        std::vector<VkSurfaceFormatKHR> const supportedSurfaceFormats = [&swapchain]() {
            uint32_t formatCount = 0;
            vkGetPhysicalDeviceSurfaceFormatsKHR(swapchain.physicalDevice, swapchain.surface, &formatCount, nullptr);
            std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(swapchain.physicalDevice, swapchain.surface, &formatCount, surfaceFormats.data());
            return surfaceFormats;
        }();

        std::vector<VkPresentModeKHR> const supportedPresentModes = [&swapchain]() {
            uint32_t presentModeCount = 0;
            vkGetPhysicalDeviceSurfacePresentModesKHR(swapchain.physicalDevice, swapchain.surface, &presentModeCount, nullptr);
            std::vector<VkPresentModeKHR> presentModes(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(swapchain.physicalDevice, swapchain.surface, &presentModeCount, presentModes.data());
            return presentModes;
        }();

        // Query preferred image count
        uint32_t const imageCount = [&surfaceCapabilities]() {
            uint32_t const imageCount = surfaceCapabilities.minImageCount + 1;
            if (surfaceCapabilities.maxImageCount == 0) {
                return imageCount;
            }

            return std::min(imageCount, surfaceCapabilities.maxImageCount);
        }();

        // Get image width and height
        uint32_t const width = std::clamp(swapchainDesc.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
        uint32_t const height = std::clamp(swapchainDesc.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);

        // Check if requested swap size is valid
        if (width == 0 || height == 0) {
            return false;
        }

        // Get surface format
        // TODO(nemjit001): Use a more sophisticated method to find required surface formats (reqs texture formats in graphics API wrapper)
        VkSurfaceFormatKHR const surfaceFormat = supportedSurfaceFormats[0];

        // Get the requested present mode from supported present modes
        VkPresentModeKHR const presentMode = [&swapchainDesc, &supportedPresentModes]() {
            if (swapchainDesc.syncInterval == 0)
            {
                // Immediate present does not wait for the vertical blanking period
                if (swapchainDesc.allowTearing
                    && std::find(supportedPresentModes.begin(), supportedPresentModes.end(), VK_PRESENT_MODE_IMMEDIATE_KHR) != supportedPresentModes.end()
                ) {
                    return VK_PRESENT_MODE_IMMEDIATE_KHR;
                }

                // Mailbox present should wait for the vertical blanking period
                if (!swapchainDesc.allowTearing
                    && std::find(supportedPresentModes.begin(), supportedPresentModes.end(), VK_PRESENT_MODE_MAILBOX_KHR) != supportedPresentModes.end()
                ) {
                    return VK_PRESENT_MODE_MAILBOX_KHR;
                }
            }
            else
            {
                // FIFO relaxed present does not wait for the vertical blanking period
                if (swapchainDesc.allowTearing
                    && std::find(supportedPresentModes.begin(), supportedPresentModes.end(), VK_PRESENT_MODE_FIFO_RELAXED_KHR) != supportedPresentModes.end()
                ) {
                    return VK_PRESENT_MODE_FIFO_RELAXED_KHR;
                }

                // FIFO present should wait for the vertical blanking period
                if (!swapchainDesc.allowTearing
                    && std::find(supportedPresentModes.begin(), supportedPresentModes.end(), VK_PRESENT_MODE_FIFO_KHR) != supportedPresentModes.end()
                ) {
                    return VK_PRESENT_MODE_FIFO_KHR;
                }
            }

            // FIFO present mode is always supported
            return VK_PRESENT_MODE_FIFO_KHR;
        }();

        // Create swapchain
        VkSwapchainCreateInfoKHR swapchainCreateInfo{};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.pNext = nullptr;
        swapchainCreateInfo.flags = 0;
        swapchainCreateInfo.surface = swapchain.surface;
        swapchainCreateInfo.minImageCount = imageCount;
        swapchainCreateInfo.imageFormat = surfaceFormat.format;
        swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
        swapchainCreateInfo.imageExtent.width = width;
        swapchainCreateInfo.imageExtent.height = height;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; //< This is pretty much the only required image usage flag for swap images
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = nullptr;
        swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; //< Opaque alpha compositing should always be supported
        swapchainCreateInfo.presentMode = presentMode;
        swapchainCreateInfo.clipped = VK_FALSE;
        swapchainCreateInfo.oldSwapchain = swapchain.swapchain;

        VkSwapchainKHR newSwapchain = VK_NULL_HANDLE;
        if (VK_CHECK_FAILED(vkCreateSwapchainKHR(swapchain.device, &swapchainCreateInfo, nullptr, &newSwapchain))) {
            return false;
        }

        // Get swapchain images
        std::vector<VkImage> const swapchainImages = [&swapchain, &newSwapchain]() {
            uint32_t imageCount = 0;
            vkGetSwapchainImagesKHR(swapchain.device, newSwapchain, &imageCount, nullptr);
            std::vector<VkImage> swapchainImages(imageCount);
            vkGetSwapchainImagesKHR(swapchain.device, newSwapchain, &imageCount, swapchainImages.data());
            return swapchainImages;
        }();

        // Create image semaphores
        std::vector<VkSemaphore> imageAvailableSemaphores;
        imageAvailableSemaphores.reserve(swapchainImages.size());
        for (size_t i = 0; i < swapchainImages.size(); i++)
        {
            VkSemaphoreCreateInfo imageSemaphoreCreateInfo{};
            imageSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            imageSemaphoreCreateInfo.pNext = nullptr;
            imageSemaphoreCreateInfo.flags = 0;

            VkSemaphore imageSemaphore = VK_NULL_HANDLE;
            if (VK_CHECK_FAILED(vkCreateSemaphore(swapchain.device, &imageSemaphoreCreateInfo, nullptr, &imageSemaphore))) {
                return false;
            }

            imageAvailableSemaphores.push_back(imageSemaphore);
        }
        
        // Destroy old swapchain objects
        DestroySwapchain(swapchain);

        // Set new swapchain state
        swapchain.width = width;
        swapchain.height = height;
        swapchain.syncInterval = swapchainDesc.syncInterval;
        swapchain.allowTearing = swapchainDesc.allowTearing;
        swapchain.swapchain = newSwapchain;
        swapchain.swapchainImages = swapchainImages;
        swapchain.imageAvailableSemaphores = imageAvailableSemaphores;
        swapchain.currentImageSemaphoreIndex = 0;
        swapchain.activeImageIndex = 0;
        swapchain.recreateSwapchain = false;
        return true;
    }

    bool ReconfigureSwapchain(VulkanSwapchain& swapchain)
    {
        // Attempt to recreate the entire swapchain by resizing to its previous size
        SPDLOG_TRACE("Reconfiguring Vulkan swapchain");
        return ResizeSwapchain(swapchain, swapchain.width, swapchain.height);
    }

    bool ResizeSwapchain(VulkanSwapchain& swapchain, uint32_t width, uint32_t height)
    {
        SPDLOG_TRACE("Resizing Vulkan swapchain to {} x {}", width, height);

        VulkanSwapchainDesc swapchainDesc{};
        swapchainDesc.physicalDevice = swapchain.physicalDevice;
        swapchainDesc.device = swapchain.device;
        swapchainDesc.surface = swapchain.surface;
        swapchainDesc.sharedDeviceState = swapchain.sharedDeviceState;
        swapchainDesc.width = width;
        swapchainDesc.height = height;
        swapchainDesc.syncInterval = swapchain.syncInterval;
        swapchainDesc.allowTearing = swapchain.allowTearing;

        bool const result = ConfigureSwapchain(swapchainDesc, swapchain);
        if (!result) {
            DestroySwapchain(swapchain);
            return false;
        }

        return true;
    }

    void DestroySwapchain(VulkanSwapchain& swapchain)
    {
        // Wait for device to be idle
        vkDeviceWaitIdle(swapchain.device);

        // Clean up image semaphores
        for (auto const& imageSemaphore : swapchain.imageAvailableSemaphores) {
            vkDestroySemaphore(swapchain.device, imageSemaphore, nullptr);
        }

        // Clean up swapchain data
        vkDestroySwapchainKHR(swapchain.device, swapchain.swapchain, nullptr);

        // Zero invalidated swapchain state
        swapchain.width = 0;
        swapchain.height = 0;
        swapchain.swapchain = VK_NULL_HANDLE;
        swapchain.swapchainImages.clear();        
        swapchain.imageAvailableSemaphores.clear();
        swapchain.currentImageSemaphoreIndex = 0;
        swapchain.activeImageIndex = 0;
    }

    bool AcquireNextImage(VulkanSwapchain& swapchain)
    {
        // Cannot render with a zero sized swapchain
        if (swapchain.width == 0 || swapchain.height == 0) {
            return false;
        }

        // Check if swapchain needs to be recreated
        if (swapchain.recreateSwapchain)
        {
            // Try to recreate swapchain once
            swapchain.recreateSwapchain = false;
            if (!ReconfigureSwapchain(swapchain)) {
                SPDLOG_WARN("Failed to recreate swapchain");
            }

            return false;
        }

        // Get next free image semaphore
        uint32_t const currentImageSemaphoreIndex = (swapchain.currentImageSemaphoreIndex + 1) % swapchain.imageAvailableSemaphores.size();
        VkSemaphore const imageAvailableSemaphore = swapchain.imageAvailableSemaphores[currentImageSemaphoreIndex];

        // Acquire next swapchain image
        uint32_t imageIndex = 0;
        VkResult const acquireResult = vkAcquireNextImageKHR(swapchain.device, swapchain.swapchain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
        if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR || acquireResult == VK_SUBOPTIMAL_KHR)
        {
            swapchain.recreateSwapchain = true;
            return false;
        }
        else if (VK_CHECK_FAILED(acquireResult))
        {
            SPDLOG_ERROR("Fatal error during swapchain image acquisition");
            return false;
        }

        // Update swapchain state
        swapchain.currentImageSemaphoreIndex = currentImageSemaphoreIndex;
        swapchain.activeImageIndex = imageIndex;
        swapchain.recreateSwapchain = false;
        return true;
    }

    void RegisterWithFrame(VulkanFrameSwapchainInfo& swapchainInfo, VulkanSwapchain const& swapchain)
    {
        swapchainInfo.swapchains.push_back(swapchain.swapchain);
        swapchainInfo.imageAvailableSemaphores.push_back(swapchain.imageAvailableSemaphores[swapchain.currentImageSemaphoreIndex]);
        swapchainInfo.imageIndices.push_back(swapchain.activeImageIndex);
        swapchainInfo.dstWaitStageMasks.push_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    }

    void PrepareForRendering(VulkanSwapchain& swapchain, VkCommandBuffer commandBuffer)
    {
        VkImageSubresourceRange subresourceRange{};
        subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        subresourceRange.baseMipLevel = 0;
        subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
        subresourceRange.baseArrayLayer = 0;
        subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

        VkImageMemoryBarrier2 swapBeginRenderingBarrier{};
        swapBeginRenderingBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        swapBeginRenderingBarrier.pNext = nullptr;
        swapBeginRenderingBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        swapBeginRenderingBarrier.srcAccessMask = 0;
        swapBeginRenderingBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        swapBeginRenderingBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        swapBeginRenderingBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        swapBeginRenderingBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        swapBeginRenderingBarrier.srcQueueFamilyIndex = swapchain.sharedDeviceState->presentQueueFamily;
        swapBeginRenderingBarrier.dstQueueFamilyIndex = swapchain.sharedDeviceState->graphicsQueueFamily;
        swapBeginRenderingBarrier.image = swapchain.swapchainImages[swapchain.activeImageIndex];
        swapBeginRenderingBarrier.subresourceRange = subresourceRange;

        VkDependencyInfo dependencyInfo{};
        dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        dependencyInfo.pNext = nullptr;
        dependencyInfo.imageMemoryBarrierCount = 1;
        dependencyInfo.pImageMemoryBarriers = &swapBeginRenderingBarrier;

        vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
    }

    void PrepareForPresent(VulkanSwapchain& swapchain, VkCommandBuffer commandBuffer)
    {
        VkImageSubresourceRange subresourceRange{};
        subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        subresourceRange.baseMipLevel = 0;
        subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
        subresourceRange.baseArrayLayer = 0;
        subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

        VkImageMemoryBarrier2 swapPresentBarrier{};
        swapPresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        swapPresentBarrier.pNext = nullptr;
        swapPresentBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        swapPresentBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        swapPresentBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        swapPresentBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
        swapPresentBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        swapPresentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        swapPresentBarrier.srcQueueFamilyIndex = swapchain.sharedDeviceState->graphicsQueueFamily;
        swapPresentBarrier.dstQueueFamilyIndex = swapchain.sharedDeviceState->presentQueueFamily;
        swapPresentBarrier.image = swapchain.swapchainImages[swapchain.activeImageIndex];
        swapPresentBarrier.subresourceRange = subresourceRange;

        VkDependencyInfo dependencyInfo{};
        dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        dependencyInfo.pNext = nullptr;
        dependencyInfo.imageMemoryBarrierCount = 1;
        dependencyInfo.pImageMemoryBarriers = &swapPresentBarrier;

        vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
    }
} // namespace Bonsai::VulkanSwapchainHelpers
