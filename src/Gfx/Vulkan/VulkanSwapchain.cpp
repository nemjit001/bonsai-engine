#include "VulkanSwapchain.hpp"

#include <algorithm>
#include <cassert>
#include <SDL3/SDL_vulkan.h>
#include <volk.h>
#include "Core/Logger.hpp"

namespace Bonsai
{
    VulkanSwapchain::VulkanSwapchain(
        VkInstance instance,
        VkPhysicalDevice physicalDevice,
        VkDevice device,
        VulkanSharedDeviceState* sharedDeviceState,
        VkSurfaceKHR surface
    )
        :
        _instance(instance),
        _physicalDevice(physicalDevice),
        _device(device),
        _sharedDeviceState(sharedDeviceState),
        _surface(surface),
        _currentImageSemaphoreIndex(0),
        _currentImageIndex(0),
        _recreateSwapchain(false),
        _currentSwapchainDesc(),
        _swapchainData()
    {
        //
    }

    VulkanSwapchain::~VulkanSwapchain()
    {
        if (_swapchainData.has_value()) {
            DestroyConfiguredSwapchain(_swapchainData.value());
        }

        vkDestroySurfaceKHR(_instance, _surface, nullptr);
    }

    VulkanSwapchain* VulkanSwapchain::TryCreate(
        SDL_Window* window,
        VkInstance instance,
        VkPhysicalDevice physicalDevice,
        VkDevice device,
        VulkanSharedDeviceState* sharedDeviceState
    )
    {
        assert(window != nullptr && "Window was nullptr!");
        assert(instance != VK_NULL_HANDLE && "Vulkan instance was VK_NULL_HANDLE!");
        assert(physicalDevice != VK_NULL_HANDLE && "Vulkan physical device was VK_NULL_HANDLE!");
        assert(device != VK_NULL_HANDLE && "Vulkan logical device was VK_NULL_HANDLE!");

        // Create surface
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        if (!SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface))
        {
            SPDLOG_ERROR("Failed to create surface");
            return nullptr;
        }

        // Check actual queue present support with the surface (this should generally not happen)
        VkBool32 hasPresentSupport = VK_FALSE;
        if (VK_CHECK_FAILED(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, sharedDeviceState->presentQueueFamily, surface, &hasPresentSupport)) || hasPresentSupport == VK_FALSE)
        {
            SPDLOG_ERROR("The present queue family does not support presenting to surfaces!");
            return nullptr;
        }

        return new VulkanSwapchain(instance, physicalDevice, device, sharedDeviceState, surface);
    }

    bool VulkanSwapchain::Configure(VulkanSwapchainDesc const& swapchainDesc)
    {
        SPDLOG_TRACE("Configuring swapchain");

        // Attempt to populate swapchain create info
        VkSwapchainCreateInfoKHR swapchainCreateInfo{};
        if (!PopulateSwapchainCreateInfo(swapchainCreateInfo, swapchainDesc))
        {
            // Release old swapchain resources on failure
            if (_swapchainData.has_value()) {
                DestroyConfiguredSwapchain(_swapchainData.value());
            }

            _swapchainData.reset();
            return false;
        }

        // Attempt to configure the swapchain
        std::optional<ConfiguredSwapchainData> swapchainData = TryConfigureSwapchain(swapchainCreateInfo);
        if (!swapchainData.has_value()) {
            return false;
        }

        // Release old swapchain resources
        if (_swapchainData.has_value()) {
            DestroyConfiguredSwapchain(_swapchainData.value());
        }

        // Set new swapchain state
        _currentSwapchainDesc = swapchainDesc;
        _swapchainData = swapchainData;

        // Cache actual swap image extent
        _currentSwapchainDesc->width = swapchainCreateInfo.imageExtent.width;
        _currentSwapchainDesc->height = swapchainCreateInfo.imageExtent.height;

        SPDLOG_TRACE("Configured swapchain");
        return true;
    }

    bool VulkanSwapchain::Resize(uint32_t width, uint32_t height)
    {
        SPDLOG_TRACE("Resizing swapchain to {} x {}", width, height);

        // Check if the swapchain was configured
        if (!_currentSwapchainDesc.has_value())
        {
            SPDLOG_WARN("Attempting to resize an unconfigured swapchain!");
            return false;
        }

        // Configure new width and height for swapchain
        VulkanSwapchainDesc swapchainDesc = _currentSwapchainDesc.value();
        swapchainDesc.width = width;
        swapchainDesc.height = height;

        return Configure(swapchainDesc);
    }

    bool VulkanSwapchain::Reconfigure()
    {
        SPDLOG_TRACE("Reconfiguring swapchain");

        // Check if the swapchain was configured
        if (!_currentSwapchainDesc.has_value())
        {
            SPDLOG_WARN("Attempting to reconfigure an unconfigured swapchain!");
            return false;
        }

        // Reconfigure with current desc
        return Configure(_currentSwapchainDesc.value());
    }

    bool VulkanSwapchain::AcquireNextImage()
    {
        // Check if swapchain is valid
        if (!IsValid()) {
            return false;
        }

        // Check if swapchain needs to be recreated
        if (_recreateSwapchain)
        {
            // Try to recreate swapchain once
            _recreateSwapchain = false;
            if (!Reconfigure()) {
                SPDLOG_WARN("Failed to reconfigure swapchain!");
            }

            return false;
        }

        // Get next free image semaphore
        uint32_t const currentImageSemaphoreIndex = (_currentImageSemaphoreIndex + 1) % _swapchainData->imageAvailableSemaphores.size();
        VkSemaphore const imageAvailableSemaphore = _swapchainData->imageAvailableSemaphores[currentImageSemaphoreIndex];

        // Acquire next swapchain image
        uint32_t imageIndex = 0;
        VkResult const acquireResult = vkAcquireNextImageKHR(_device, _swapchainData->swapchain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
        if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR || acquireResult == VK_SUBOPTIMAL_KHR)
        {
            _recreateSwapchain = true;
            return false;
        }
        else if (VK_CHECK_FAILED(acquireResult))
        {
            SPDLOG_ERROR("Fatal error during swapchain image acquisition");
            return false;
        }

        // Update swapchain state
        _currentImageSemaphoreIndex = currentImageSemaphoreIndex;
        _currentImageIndex = imageIndex;
        return true;
    }

    void VulkanSwapchain::RegisterWithFrame(VulkanFrameSwapchainInfo& swapchainInfo)
    {
        // Check if swapchain is valid
        if (!IsValid()) {
            return;
        }

        // Store data in frame swapchain info
        swapchainInfo.swapchains.push_back(_swapchainData->swapchain);
        swapchainInfo.imageAvailableSemaphores.push_back(_swapchainData->imageAvailableSemaphores[_currentImageSemaphoreIndex]);
        swapchainInfo.imageIndices.push_back(_currentImageIndex);
        swapchainInfo.dstWaitStageMasks.push_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    }

    void VulkanSwapchain::PrepareForRendering(VkCommandBuffer commandBuffer)
    {
        // Check if swapchain is valid
        if (!IsValid()) {
            return;
        }

        // Set up image barrier
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
        swapBeginRenderingBarrier.srcQueueFamilyIndex = _sharedDeviceState->presentQueueFamily;
        swapBeginRenderingBarrier.dstQueueFamilyIndex = _sharedDeviceState->graphicsQueueFamily;
        swapBeginRenderingBarrier.image = _swapchainData->swapchainImages[_currentImageIndex];
        swapBeginRenderingBarrier.subresourceRange = subresourceRange;

        VkDependencyInfo dependencyInfo{};
        dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        dependencyInfo.pNext = nullptr;
        dependencyInfo.imageMemoryBarrierCount = 1;
        dependencyInfo.pImageMemoryBarriers = &swapBeginRenderingBarrier;

        vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
    }

    void VulkanSwapchain::PrepareForPresent(VkCommandBuffer commandBuffer)
    {
        // Check if swapchain is valid
        if (!IsValid()) {
            return;
        }

        // Set up image barrier
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
        swapPresentBarrier.srcQueueFamilyIndex = _sharedDeviceState->graphicsQueueFamily;
        swapPresentBarrier.dstQueueFamilyIndex = _sharedDeviceState->presentQueueFamily;
        swapPresentBarrier.image = _swapchainData->swapchainImages[_currentImageIndex];
        swapPresentBarrier.subresourceRange = subresourceRange;

        VkDependencyInfo dependencyInfo{};
        dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        dependencyInfo.pNext = nullptr;
        dependencyInfo.imageMemoryBarrierCount = 1;
        dependencyInfo.pImageMemoryBarriers = &swapPresentBarrier;

        vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
    }

    bool VulkanSwapchain::IsValid() const
    {
        return _swapchainData.has_value();
    }

    uint32_t VulkanSwapchain::DetermineSwapImageCount(VkSurfaceCapabilitiesKHR const& surfaceCapabilities)
    {
        uint32_t const swapImageCount = surfaceCapabilities.minImageCount + 1;
        if (surfaceCapabilities.maxImageCount == 0) {
            return swapImageCount;
        }

        return std::min(swapImageCount, surfaceCapabilities.maxImageCount);
    }

    VkPresentModeKHR VulkanSwapchain::DeterminePresentMode(std::vector<VkPresentModeKHR> const& supportedPresentModes, uint32_t syncInterval, bool allowTearing)
    {
        if (syncInterval == 0)
            {
                // Immediate present does not wait for the vertical blanking period
                if (allowTearing
                    && std::find(supportedPresentModes.begin(), supportedPresentModes.end(), VK_PRESENT_MODE_IMMEDIATE_KHR) != supportedPresentModes.end()
                ) {
                    return VK_PRESENT_MODE_IMMEDIATE_KHR;
                }

                // Mailbox present should wait for the vertical blanking period
                if (!allowTearing
                    && std::find(supportedPresentModes.begin(), supportedPresentModes.end(), VK_PRESENT_MODE_MAILBOX_KHR) != supportedPresentModes.end()
                ) {
                    return VK_PRESENT_MODE_MAILBOX_KHR;
                }
            }
            else
            {
                // FIFO relaxed present does not wait for the vertical blanking period
                if (allowTearing
                    && std::find(supportedPresentModes.begin(), supportedPresentModes.end(), VK_PRESENT_MODE_FIFO_RELAXED_KHR) != supportedPresentModes.end()
                ) {
                    return VK_PRESENT_MODE_FIFO_RELAXED_KHR;
                }

                // FIFO present should wait for the vertical blanking period
                if (!allowTearing
                    && std::find(supportedPresentModes.begin(), supportedPresentModes.end(), VK_PRESENT_MODE_FIFO_KHR) != supportedPresentModes.end()
                ) {
                    return VK_PRESENT_MODE_FIFO_KHR;
                }
            }

            // FIFO present mode is always supported
            return VK_PRESENT_MODE_FIFO_KHR;
    }

    bool VulkanSwapchain::PopulateSwapchainCreateInfo(VkSwapchainCreateInfoKHR& swapchainCreateInfo, VulkanSwapchainDesc const& swapchainDesc)
    {
        SPDLOG_TRACE("Populating swapchain create info");

        // Check for zero sized swapchain
        if (swapchainDesc.width == 0 || swapchainDesc.height == 0)
        {
            SPDLOG_WARN("Trying to configure a zero sized swapchain!");
            return false;
        }

        // Query surface capabilities
        VkSurfaceCapabilitiesKHR surfaceCapabilities{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevice, _surface, &surfaceCapabilities);

        // Query surface formats and present modes
        std::vector<VkSurfaceFormatKHR> const supportedSurfaceFormats = [this]() {
            uint32_t formatCount = 0;
            vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, _surface, &formatCount, nullptr);
            std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, _surface, &formatCount, surfaceFormats.data());
            return surfaceFormats;
        }();

        std::vector<VkPresentModeKHR> const supportedPresentModes = [this]() {
            uint32_t presentModeCount = 0;
            vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, _surface, &presentModeCount, nullptr);
            std::vector<VkPresentModeKHR> presentModes(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, _surface, &presentModeCount, presentModes.data());
            return presentModes;
        }();

        // Get image count
        uint32_t const imageCount = DetermineSwapImageCount(surfaceCapabilities);
        if (imageCount == 0)
        {
            SPDLOG_WARN("Swap image count was 0!");
            return false;
        }

        // Get image width and height
        uint32_t const width = std::clamp(swapchainDesc.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
        uint32_t const height = std::clamp(swapchainDesc.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
        if (width == 0 || height == 0)
        {
            SPDLOG_WARN("Swap image extent was 0 x 0!");
            return false;
        }

        // Get surface format
        // TODO(nemjit001): Use a more sophisticated method to find required surface formats (reqs texture formats in graphics API wrapper)
        VkSurfaceFormatKHR const surfaceFormat = supportedSurfaceFormats[0];

        // Get the requested present mode from supported present modes
        VkPresentModeKHR const presentMode = DeterminePresentMode(supportedPresentModes, swapchainDesc.syncInterval, swapchainDesc.allowTearing);

        // Populate create info
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.pNext = nullptr;
        swapchainCreateInfo.flags = 0;
        swapchainCreateInfo.surface = _surface;
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
        swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

        // Set old swapchain if it exists
        if (_swapchainData.has_value()) {
            swapchainCreateInfo.oldSwapchain = _swapchainData->swapchain;
        }

        SPDLOG_TRACE("Populated swapchain create info:");
        SPDLOG_TRACE("\timage count: {}", imageCount);
        SPDLOG_TRACE("\timage extent: {} x {}", width, height);
        return true;
    }

    std::optional<VulkanSwapchain::ConfiguredSwapchainData> VulkanSwapchain::TryConfigureSwapchain(VkSwapchainCreateInfoKHR const& swapchainCreateInfo)
    {
        // Create swapchain
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        if (VK_CHECK_FAILED(vkCreateSwapchainKHR(_device, &swapchainCreateInfo, nullptr, &swapchain))) {
            return {};
        }

        // Get swapchain images
        std::vector<VkImage> const swapchainImages = [this, &swapchain]() {
            uint32_t imageCount = 0;
            vkGetSwapchainImagesKHR(_device, swapchain, &imageCount, nullptr);
            std::vector<VkImage> swapchainImages(imageCount);
            vkGetSwapchainImagesKHR(_device, swapchain, &imageCount, swapchainImages.data());
            return swapchainImages;
        }();

        // Create image semaphores
        std::vector<VkSemaphore> imageAvailableSemaphores;
        imageAvailableSemaphores.reserve(swapchainImages.size());
        for (size_t i = 0; i < swapchainImages.size(); i++)
        {
            VkSemaphoreCreateInfo semaphoreCreateInfo{};
            semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            semaphoreCreateInfo.pNext = nullptr;
            semaphoreCreateInfo.flags = 0;

            VkSemaphore imageSemaphore = VK_NULL_HANDLE;
            if (VK_CHECK_FAILED(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &imageSemaphore))) {
                return {};
            }

            imageAvailableSemaphores.push_back(imageSemaphore);
        }

        // Construct configured swapchain data
        return ConfiguredSwapchainData {
            swapchain,
            swapchainImages,
            imageAvailableSemaphores,
        };
    }

    void VulkanSwapchain::DestroyConfiguredSwapchain(ConfiguredSwapchainData const& swapchainData)
    {
        // Wait for device idle before cleaning up any resources
        vkDeviceWaitIdle(_device);

        // Clean up image semaphores
        for (auto const& imageSemaphore : swapchainData.imageAvailableSemaphores) {
            vkDestroySemaphore(_device, imageSemaphore, nullptr);
        }

        // Destroy swapchain
        vkDestroySwapchainKHR(_device, swapchainData.swapchain, nullptr);
    }
} // namespace Bonsai
