#pragma once
#ifndef BONSAI_GRAPHICS_DEVICE_HPP
#define BONSAI_GRAPHICS_DEVICE_HPP

#include <optional>
#include <volk.h>

#define VK_CHECK_SUCCEEDED(result)  (result == VK_SUCCESS)
#define VK_CHECK_FAILED(result)     (result != VK_SUCCESS)

namespace Bonsai
{
    class GraphicsDevice
    {
    public:
        virtual ~GraphicsDevice();

        GraphicsDevice(GraphicsDevice const&) = delete;
        GraphicsDevice& operator=(GraphicsDevice const&) = delete;

        /// @brief Try to create a graphics device.
        /// @return The newly created graphics device, or nullptr on failure.
        [[nodiscard]] static GraphicsDevice* TryCreate();

    private:
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
            uint32_t graphicsQueueFamily;
            uint32_t computeQueueFamily;
            uint32_t transferQueueFamily;
        };

        /// @brief Reference API version against which the graphics implementation is written.
        static constexpr uint32_t ReferenceVulkanApiVersion = VK_API_VERSION_1_3;

        /// @brief Constructor for graphics device.
        /// @param instanceState Initialized instance state.
        /// @param physicalDeviceInfo Initialized physical device info.
        GraphicsDevice(VulkanInstanceState const& instanceState, VulkanPhysicalDeviceInfo const& physicalDeviceInfo);

        /// @brief Create Vulkan instance level state.
        /// @return An optional Vulkan instance state structure.
        [[nodiscard]] static std::optional<VulkanInstanceState> CreateInstanceState();

        /// @brief Find a Vulkan physical device that supports the application requirements.
        /// @param instanceState Instance state to use for device query.
        /// @return An optional Vulkan physical device info structure.
        [[nodiscard]] static std::optional<VulkanPhysicalDeviceInfo> FindPhysicalDevice(VulkanInstanceState const& instanceState);
        
        /// @brief Destroy instance state.
        /// @param instanceState Instance state.
        static void DestroyInstanceState(VulkanInstanceState const& instanceState);

    private:
        VulkanInstanceState _instanceState{};
        VulkanPhysicalDeviceInfo _physicalDeviceInfo{};
    };
} // namespace Bonsai

#endif //BONSAI_GRAPHICS_DEVICE_HPP
