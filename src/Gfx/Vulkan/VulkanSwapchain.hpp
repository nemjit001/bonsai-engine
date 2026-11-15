#pragma once
#ifndef BONSAI_VULKAN_SWAPCHAIN_HPP
#define BONSAI_VULKAN_SWAPCHAIN_HPP

#define VK_NO_PROTOTYPES

#include <optional>
#include <vector>
#include <SDL3/SDL.h>
#include <vulkan/vulkan.h>
#include "Gfx/Vulkan/VulkanGraphicsTypes.hpp"

namespace Bonsai
{
    struct VulkanSwapchainDesc
    {
        GraphicsFormat preferredFormat;
        GraphicsFormat fallbackFormat;
        uint32_t width;
        uint32_t height;
        uint32_t syncInterval;
        bool allowTearing;
    };

    class VulkanSwapchain : public GraphicsSwapchain
    {
    private:
        VulkanSwapchain(
            VkInstance instance,
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            VulkanSharedDeviceState* sharedDeviceState,
            VkSurfaceKHR surface
        );

    protected:
        ~VulkanSwapchain() override;

    public:
        /// @brief Try to create a Vulkan swapchain.
        /// @param window Window to use for swapchain creation.
        /// @param instance Instance to use in the swapchain.
        /// @param physicalDevice Physical device to use in the swapchain.
        /// @param device Logical device to use in the swapchain.
        /// @param sharedDeviceState Shared device state.
        /// @return 
        [[nodiscard]] static VulkanSwapchain* TryCreate(
            SDL_Window* window,
            VkInstance instance,
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            VulkanSharedDeviceState* sharedDeviceState
        );

        /// @brief Configure the swapchain.
        /// @param swapchainDesc Swapchain description.
        /// @return A boolean indicating success.
        [[nodiscard]] bool Configure(VulkanSwapchainDesc const& swapchainDesc);

        /// @brief Resize the swapchain to a given size.
        /// @param width New swapchain width.
        /// @param height New swapchain height.
        /// @return A boolean indicating success.
        [[nodiscard]] bool Resize(uint32_t width, uint32_t height);

        /// @brief Reconfigure the swapchain using its current configuration.
        /// @return A boolean indicating success.
        [[nodiscard]] bool Reconfigure();

        /// @brief Acquire the next swapchain image.
        /// @return A boolean indicating success.
        [[nodiscard]] bool AcquireNextImage();

        /// @brief Register this swapchain's data into the Vulkan frame swapchain info.
        /// @param swapchainInfo Swapchain info to register with.
        void RegisterWithFrame(VulkanFrameSwapchainInfo& swapchainInfo);

        /// @brief Prepare the current active swap image for rendering.
        /// @param commandBuffer 
        void PrepareForRendering(VkCommandBuffer commandBuffer);

        /// @brief Prepare the current active swap image for presentation.
        /// @param commandBuffer 
        void PrepareForPresent(VkCommandBuffer commandBuffer);

        /// @brief Check if the swapchain is valid.
        /// @return 
        bool IsValid() const;

    private:
        /// @brief Data for a configured swapchain.
        struct ConfiguredSwapchainData
        {
            GraphicsFormat swapFormat;
            VkSwapchainKHR swapchain;
            std::vector<VkImage> swapchainImages;
            std::vector<VkImageView> swapchainImageViews;
            std::vector<VkSemaphore> imageAvailableSemaphores;
        };

        /// @brief Determine the required swap image count.
        /// @param surfaceCapabilities Current surface capabilities.
        /// @return The number of required swap images.
        [[nodiscard]] static uint32_t DetermineSwapImageCount(VkSurfaceCapabilitiesKHR const& surfaceCapabilities);

        /// @brief Determine the surface format based on a preferred and fallback format.
        /// @param availableSurfaceFormats Supported formats.
        /// @param preferredFormat First choice.
        /// @param fallbackFormat Fallback choice.
        /// @return An optional surface format.
        [[nodiscard]] static std::optional<GraphicsFormat> DetermineSurfaceFormat(std::vector<VkSurfaceFormatKHR> const& availableSurfaceFormats, GraphicsFormat preferredFormat, GraphicsFormat fallbackFormat);

        /// @brief Determine the swap present mode based on support.
        /// @param availablePresentModes Supported modes.
        /// @param syncInterval Required sync interval.
        /// @param allowTearing Whether screen tearing is allowed.
        /// @return The present mode.
        [[nodiscard]] static VkPresentModeKHR DeterminePresentMode(std::vector<VkPresentModeKHR> const& availablePresentModes, uint32_t syncInterval, bool allowTearing);

        /// @brief Populate Vulkan swapchain create info.
        /// @param swapchainCreateInfo Target swapchain create info struct.
        /// @param swapchainDesc Swapchain desc to use for populating the create info.
        /// @return A boolean indicating success.
        [[nodiscard]] bool PopulateSwapchainCreateInfo(VkSwapchainCreateInfoKHR& swapchainCreateInfo, VulkanSwapchainDesc const& swapchainDesc);

        /// @brief Attempt to configure the swapchain.
        /// @param swapchainCreateInfo Swapchain create info.
        /// @return An optional configure swapchain.
        [[nodiscard]] std::optional<ConfiguredSwapchainData> TryConfigureSwapchain(VkSwapchainCreateInfoKHR const& swapchainCreateInfo);

        /// @brief Destroy a configured swapchain.
        /// @param swapchainData Swapchain data to destroy.
        void DestroyConfiguredSwapchain(ConfiguredSwapchainData const& swapchainData);

        VkInstance _instance;
        VkPhysicalDevice _physicalDevice;
        VkDevice _device;
        VulkanSharedDeviceState* _sharedDeviceState;

        VkSurfaceKHR _surface;
        uint32_t _currentImageSemaphoreIndex;
        uint32_t _currentImageIndex;
        bool _recreateSwapchain;
        std::optional<VulkanSwapchainDesc> _currentSwapchainDesc;
        std::optional<ConfiguredSwapchainData> _swapchainData;
    };
} // namespace Bonsai

#endif //BONSAI_VULKAN_SWAPCHAIN_HPP
