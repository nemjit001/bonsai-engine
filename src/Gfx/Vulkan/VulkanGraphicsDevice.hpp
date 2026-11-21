#pragma once
#ifndef BONSAI_VULKAN_GRAPHICS_DEVICE_HPP
#define BONSAI_VULKAN_GRAPHICS_DEVICE_HPP

#define VK_NO_PROTOTYPES

#include <cstddef>
#include <memory>
#include <optional>
#include <vector>
#include <vulkan/vulkan.h>
#include "Gfx/Internal/GraphicsDeviceImpl.hpp"

namespace Bonsai
{
    struct VulkanSharedDeviceState;
    class VulkanObjectFactory;
    class VulkanRenderContext;
    class VulkanTransferContext;

    class VulkanGraphicsDevice : public GraphicsDeviceImpl
    {
    private:
        struct VulkanInstanceState;
        struct VulkanPhysicalDeviceInfo;
        struct VulkanDeviceState;

        /// @brief Constructor for graphics device.
        /// @param instanceState Initialized instance state.
        /// @param physicalDeviceInfo Initialized physical device info.
        /// @param deviceState Initialized device state.
        /// @param commandBufferPool Initialized command buffer pool.
        /// @param renderContext Initialized render context.
        /// @param transferContext Initialized transfer context.
        /// @param objectFactory Initialized object factory.
        VulkanGraphicsDevice(
            VulkanInstanceState const& instanceState,
            VulkanPhysicalDeviceInfo const& physicalDeviceInfo,
            VulkanDeviceState const& deviceState,
            std::unique_ptr<VulkanSharedDeviceState>&& sharedDeviceState,
            std::unique_ptr<VulkanRenderContext>&& renderContext,
            std::unique_ptr<VulkanTransferContext>&& transferContext,
            std::unique_ptr<VulkanObjectFactory>&& objectFactory
        );

    public:
        ~VulkanGraphicsDevice() override;

        /// @brief Try to create a Vulkan graphics device.
        /// @return The newly created Vulkan graphics device, or nullptr on failure.
        [[nodiscard]] static VulkanGraphicsDevice* TryCreate();

        [[nodiscard]] GraphicsBuffer* CreateBuffer(GraphicsBufferDesc const& bufferDesc) override final;
        [[nodiscard]] GraphicsTexture* CreateTexture(GraphicsTextureDesc const& textureDesc) override final;
        [[nodiscard]] GraphicsTextureView* CreateTextureView(GraphicsTextureViewDesc const& textureViewDesc) override final;
        [[nodiscard]] GraphicsSampler* CreateSampler(GraphicsSamplerDesc const& samplerDesc) override final;
        [[nodiscard]] GraphicsDescriptorSetLayout* CreateDescriptorSetLayout(GraphicsDescriptorSetLayoutDesc const& descriptorSetLayoutDesc) override final;
        [[nodiscard]] GraphicsDescriptorSet* CreateDescriptorSet(FrameContext* frameContext, GraphicsDescriptorSetDesc const& descriptorSetDesc) override final;
        [[nodiscard]] GraphicsPipelineLayout* CreatePipelineLayout(GraphicsPipelineLayoutDesc const& pipelineLayoutDesc) override final;
        [[nodiscard]] GraphicsRenderPipeline* CreateRenderPipeline(GraphicsRenderPipelineDesc const& renderPipelineDesc) override final;
        [[nodiscard]] GraphicsComputePipeline* CreateComputePipeline(GraphicsComputePipelineDesc const& computePipelineDesc) override final;
        [[nodiscard]] GraphicsSwapchain* CreateSwapchain(GraphicsSwapchainDesc const& swapchainDesc) override final;

        [[nodiscard]] FrameContext* StartFrame(GraphicsSwapchain** swapchains, size_t swapchainCount) override final;
        void SubmitFrame(FrameContext* frameContext) override final;

        [[nodiscard]] TransferBatch* StartTransferBatch() override final;
        void SubmitTransferBatch(TransferBatch* transferBatch) override final;
        void WaitForTransferBatch(TransferBatch* transferBatch) override final;

        void ResizeSwapchain(GraphicsSwapchain* swapchain, uint32_t width, uint32_t height) override final;

        void WaitForIdle() override final;

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
            VkPhysicalDeviceMemoryProperties2 physicalDeviceMemoryProperties2;
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
        std::unique_ptr<VulkanTransferContext> _transferContext;
        std::unique_ptr<VulkanObjectFactory> _objectFactory;
    };
} // namespace Bonsai

#endif //BONSAI_VULKAN_GRAPHICS_DEVICE_HPP
