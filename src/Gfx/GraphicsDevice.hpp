#pragma once
#ifndef BONSAI_GRAPHICS_DEVICE_HPP
#define BONSAI_GRAPHICS_DEVICE_HPP

#include <optional>
#include <vector>
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

        /// @brief Vulkan device level state.
        struct VulkanDeviceState
        {
            VkDevice device;
            VkQueue graphicsQueue;
            VkQueue computeQueue;
            VkQueue transferQueue;
        };

        /// @brief Reference API version against which the graphics implementation is written.
        static constexpr uint32_t ReferenceVulkanApiVersion = VK_API_VERSION_1_3;

        /// @brief Constructor for graphics device.
        /// @param instanceState Initialized instance state.
        /// @param physicalDeviceInfo Initialized physical device info.
        /// @param deviceState Initialized device state.
        GraphicsDevice(
            VulkanInstanceState const& instanceState,
            VulkanPhysicalDeviceInfo const& physicalDeviceInfo,
            VulkanDeviceState const& deviceState
        );

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
        VulkanInstanceState _instanceState{};
        VulkanPhysicalDeviceInfo _physicalDeviceInfo{};
        VulkanDeviceState _deviceState{};
    };
} // namespace Bonsai

#endif //BONSAI_GRAPHICS_DEVICE_HPP
