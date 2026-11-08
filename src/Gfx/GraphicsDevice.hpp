#pragma once
#ifndef BONSAI_GRAPHICS_DEVICE_HPP
#define BONSAI_GRAPHICS_DEVICE_HPP

#define VK_NO_PROTOTYPES

#include <cstddef>
#include <memory>
#include <optional>
#include <vector>
#include <vulkan/vulkan.h>
#include "Gfx/GraphicsTypes.hpp"

namespace Bonsai
{
    struct VulkanSharedDeviceState;
    class VulkanRenderContext;
    class VulkanObjectFactory;

    class GraphicsDevice
    {
    private:
        struct VulkanInstanceState;
        struct VulkanPhysicalDeviceInfo;
        struct VulkanDeviceState;

        /// @brief Constructor for graphics device.
        /// @param instanceState Initialized instance state.
        /// @param physicalDeviceInfo Initialized physical device info.
        /// @param deviceState Initialized device state.
        /// @param renderContext Initialized render context.
        /// @param objectFactory Initialized object factory.
        GraphicsDevice(
            VulkanInstanceState const& instanceState,
            VulkanPhysicalDeviceInfo const& physicalDeviceInfo,
            VulkanDeviceState const& deviceState,
            std::unique_ptr<VulkanSharedDeviceState>&& sharedDeviceState,
            std::unique_ptr<VulkanRenderContext>&& renderContext,
            std::unique_ptr<VulkanObjectFactory>&& objectFactory
        );

    public:
        virtual ~GraphicsDevice();

        GraphicsDevice(GraphicsDevice const&) = delete;
        GraphicsDevice& operator=(GraphicsDevice const&) = delete;

        /// @brief Try to create a graphics device.
        /// @return The newly created graphics device, or nullptr on failure.
        [[nodiscard]] static GraphicsDevice* TryCreate();

        /// @brief Create a swapchain on this graphics device.
        /// @param swapchainDesc Swapchain description.
        /// @return A new graphics swapchain, or nullptr on failure.
        [[nodiscard]] GraphicsSwapchain* CreateSwapchain(GraphicsSwapchainDesc const& swapchainDesc);

        /// @brief Destroy a swapchain.
        /// @param swapchain Swapchain to destroy.
        void DestroySwapchain(GraphicsSwapchain* swapchain);

        /// @brief Start a new frame on the graphics device.
        /// @param swapchain Swapchains participating in this frame.
        /// @param swapchainCount Number of swapchains in the swapchains array.
        /// @return The frame context for the started frame.
        [[nodiscard]] FrameContext* StartFrame(GraphicsSwapchain** swapchains, size_t swapchainCount);

        /// @brief Submit a frame to the graphics device.
        /// @param frameContext Frame context to submit.
        void SubmitFrame(FrameContext* frameContext);

        /// @brief Resize a swapchain.
        /// @param swapchain Swapchain to resize.
        /// @param width New swapchain width.
        /// @param height New swapchain height.
        void ResizeSwapchain(GraphicsSwapchain* swapchain, uint32_t width, uint32_t height);

        /// @brief Wait for the graphics device to be idle.
        void WaitForIdle();

    private:
        /// @brief Reference API version against which the graphics implementation is written.
        static constexpr uint32_t ReferenceVulkanApiVersion = VK_API_VERSION_1_3;
        /// @brief Number of frames that can be recorded simultaneously.
        static constexpr uint32_t FramesInFlight = 2u;

        /// @brief Vulkan instance level state.
        struct VulkanInstanceState
        {
            VkInstance instance = VK_NULL_HANDLE;
            VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
        };

        /// @brief Vulkan physical device info.
        struct VulkanPhysicalDeviceInfo
        {
            VkPhysicalDevice physicalDevice;
            bool isSupported;
            VkPhysicalDeviceProperties2 physicalDeviceProperties2;
            VkPhysicalDeviceFeatures2 physicalDeviceFeatures2;
            VkPhysicalDeviceVulkan11Features physicalDeviceVulkan11Features;
            VkPhysicalDeviceVulkan12Features physicalDeviceVulkan12Features;
            VkPhysicalDeviceVulkan13Features physicalDeviceVulkan13Features;
            uint32_t graphicsQueueFamily;
            uint32_t presentQueueFamily;
            uint32_t computeQueueFamily;
            uint32_t transferQueueFamily;
        };

        /// @brief Vulkan device level state.
        struct VulkanDeviceState
        {
            VkDevice device;
            VkQueue graphicsQueue;
            VkQueue presentQueue;
            VkQueue computeQueue;
            VkQueue transferQueue;
        };

        /// @brief Create Vulkan instance level state.
        /// @return An optional Vulkan instance state structure.
        [[nodiscard]] static std::optional<VulkanInstanceState> CreateInstanceState();

        /// @brief Find a Vulkan physical device that supports the application requirements.
        /// @param instanceState Instance state to use for device query.
        /// @return An optional Vulkan physical device info structure.
        [[nodiscard]] static std::optional<VulkanPhysicalDeviceInfo> FindPhysicalDevice(VulkanInstanceState const& instanceState);

        /// @brief Create Vulkan device level state.
        /// @param physicalDeviceInfo Physical device info used for device state creation.
        /// @return An optional Vulkan device state structure.
        [[nodiscard]] static std::optional<VulkanDeviceState> CreateDeviceState(VulkanPhysicalDeviceInfo const& physicalDeviceInfo);
        
        /// @brief Destroy instance state.
        /// @param instanceState Instance state.
        static void DestroyInstanceState(VulkanInstanceState const& instanceState);

        /// @brief Destroy device state.
        /// @param deviceState Device state.
        static void DestroyDeviceState(VulkanDeviceState const& deviceState);

        /// @brief Get the list of unique queue family indices for a physical device.
        /// @param physicalDeviceInfo Physical device info with queue families.
        /// @return A list of unique queue families.
        [[nodiscard]] static std::vector<uint32_t> GetUniqueQueueFamilyIndices(VulkanPhysicalDeviceInfo const& physicalDeviceInfo);

    private:
        VulkanInstanceState _instanceState;
        VulkanPhysicalDeviceInfo _physicalDeviceInfo;
        VulkanDeviceState _deviceState;
        std::unique_ptr<VulkanSharedDeviceState> _sharedDeviceState;
        std::unique_ptr<VulkanRenderContext> _renderContext;
        std::unique_ptr<VulkanObjectFactory> _objectFactory;
    };
} // namespace Bonsai

#endif //BONSAI_GRAPHICS_DEVICE_HPP
