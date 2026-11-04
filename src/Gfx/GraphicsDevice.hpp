#pragma once
#ifndef BONSAI_GRAPHICS_DEVICE_HPP
#define BONSAI_GRAPHICS_DEVICE_HPP

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

        /// @brief Reference API version against which the graphics implementation is written.
        static constexpr uint32_t ReferenceVulkanApiVersion = VK_API_VERSION_1_3;

        /// @brief Constructor for graphics device.
        /// @param instanceState 
        GraphicsDevice(VulkanInstanceState const& instanceState);

        /// @brief Create Vulkan instance level state.
        /// @param instanceState Instance state.
        /// @return A boolean indicating successful state creation.
        [[nodiscard]] static bool CreateInstanceState(VulkanInstanceState& instanceState);
        
        /// @brief Destroy instance state.
        /// @param instanceState Instance state.
        static void DestroyInstanceState(VulkanInstanceState const& instanceState);

    private:
        VulkanInstanceState _instanceState{};
    };
} // namespace Bonsai

#endif //BONSAI_GRAPHICS_DEVICE_HPP
