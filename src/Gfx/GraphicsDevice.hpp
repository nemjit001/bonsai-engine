#pragma once
#ifndef BONSAI_GRAPHICS_DEVICE_HPP
#define BONSAI_GRAPHICS_DEVICE_HPP

#define VK_NO_PROTOTYPES

#include <cstddef>
#include <memory>
#include <optional>
#include <vector>
#include <vulkan/vulkan.h>
#include "Gfx/GraphicsCommandBuffer.hpp"
#include "Gfx/GraphicsTypes.hpp"

namespace Bonsai
{
    struct VulkanSharedDeviceState;
    class VulkanObjectFactory;
    class VulkanRenderContext;
    class VulkanTransferContext;

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
        /// @param transferContext Initialized transfer context.
        /// @param objectFactory Initialized object factory.
        GraphicsDevice(
            VulkanInstanceState const& instanceState,
            VulkanPhysicalDeviceInfo const& physicalDeviceInfo,
            VulkanDeviceState const& deviceState,
            std::unique_ptr<VulkanSharedDeviceState>&& sharedDeviceState,
            std::unique_ptr<VulkanRenderContext>&& renderContext,
            std::unique_ptr<VulkanTransferContext>&& transferContext,
            std::unique_ptr<VulkanObjectFactory>&& objectFactory
        );

    public:
        virtual ~GraphicsDevice();

        GraphicsDevice(GraphicsDevice const&) = delete;
        GraphicsDevice& operator=(GraphicsDevice const&) = delete;

        /// @brief Try to create a graphics device.
        /// @return The newly created graphics device, or nullptr on failure.
        [[nodiscard]] static GraphicsDevice* TryCreate();

        /// @brief Destroy a graphics device.
        /// @param graphicsDevice Graphics device to destroy.
        static void Destroy(GraphicsDevice* graphicsDevice);

        /// @brief Create a buffer using this graphics device.
        /// @param bufferDesc Buffer description.
        /// @return A new graphics buffer, or nullptr on failure.
        [[nodiscard]] GraphicsBuffer* CreateBuffer(GraphicsBufferDesc const& bufferDesc);

        /// @brief Create a texture using this graphics device.
        /// @param textureDesc Texture description.
        /// @return A new graphics texture, or nullptr on failure.
        [[nodiscard]] GraphicsTexture* CreateTexture(GraphicsTextureDesc const& textureDesc);

        /// @brief Create a texture view using this graphics device.
        /// @param textureViewDesc Texture view description.
        /// @return A new graphics texture view, or nullptr on failure.
        [[nodiscard]] GraphicsTextureView* CreateTextureView(GraphicsTextureViewDesc const& textureViewDesc);

        /// @brief Create a sampler using this graphics device.
        /// @param samplerDesc Sampler description.
        /// @return A new graphics sampler, or nullptr on failure.
        [[nodiscard]] GraphicsSampler* CreateSampler(GraphicsSamplerDesc const& samplerDesc);

        /// @brief Create a descriptor set layout using this graphics device.
        /// @param descriptorSetLayoutDesc Descriptor set layout description.
        /// @return A new descriptor set layout, or nullptr on failure.
        [[nodiscard]] GraphicsDescriptorSetLayout* CreateDescriptorSetLayout(GraphicsDescriptorSetLayoutDesc const& descriptorSetLayoutDesc);
 
        /// @brief Create a descriptor set using this graphics device. Descriptor sets are invalidated after frame completion.
        /// @param frameContext Frame context to allocate a descriptor set for.
        /// @param descriptorSetDesc Descriptor set description.
        /// @return A new descriptor set, or nullptr on failure.
        [[nodiscard]] GraphicsDescriptorSet* CreateDescriptorSet(FrameContext* frameContext, GraphicsDescriptorSetDesc const& descriptorSetDesc);

        /// @brief Create a pipeline layout using this graphics device.
        /// @param pipelineLayoutDesc Pipeline description.
        /// @return A new graphics pipeline layout, or nullptr on failure.
        [[nodiscard]] GraphicsPipelineLayout* CreatePipelineLayout(GraphicsPipelineLayoutDesc const& pipelineLayoutDesc);

        /// @brief Create a render pipeline using this graphics device.
        /// @param renderPipelineDesc Render pipeline description.
        /// @return A new graphics render pipeline, or nullptr on failure.
        [[nodiscard]] GraphicsRenderPipeline* CreateRenderPipeline(GraphicsRenderPipelineDesc const& renderPipelineDesc);

        /// @brief Create a compute pipeline using this graphics device.
        /// @param computePipelineDesc Compute pipeline description.
        /// @return A new graphics compute pipeline, or nullptr on failure.
        [[nodiscard]] GraphicsComputePipeline* CreateComputePipeline(GraphicsComputePipelineDesc const& computePipelineDesc);

        /// @brief Create a swapchain using this graphics device.
        /// @param swapchainDesc Swapchain description.
        /// @return A new graphics swapchain, or nullptr on failure.
        [[nodiscard]] GraphicsSwapchain* CreateSwapchain(GraphicsSwapchainDesc const& swapchainDesc);

        /// @brief Destroy a buffer.
        /// @param buffer Buffer to destroy.
        void DestroyBuffer(GraphicsBuffer* buffer);

        /// @brief Destroy a texture.
        /// @param texture Texture to destroy.
        void DestroyTexture(GraphicsTexture* texture);

        /// @brief Destroy a texture view.
        /// @param textureView Texture view to destroy.
        void DestroyTextureView(GraphicsTextureView* textureView);

        /// @brief Destroy a sampler.
        /// @param sampler Sampler to destroy.
        void DestroySampler(GraphicsSampler* sampler);

        /// @brief Destroy a descriptor set layout.
        /// @param descriptorSetLayout Descriptor set layout to destroy.
        void DestroyDescriptorSetLayout(GraphicsDescriptorSetLayout* descriptorSetLayout);

        /// @brief Destroy a descriptor set.
        /// @param descriptorSet Descriptor set to destroy.
        void DestroyDescriptorSet(GraphicsDescriptorSet* descriptorSet);

        /// @brief Destroy a pipeline layout.
        /// @param pipelineLayout Pipeline layout to destroy.
        void DestroyPipelineLayout(GraphicsPipelineLayout* pipelineLayout);

        /// @brief Destroy a render pipeline.
        /// @param renderPipeline Render pipeline to destroy.
        void DestroyRenderPipeline(GraphicsRenderPipeline* renderPipeline);

        /// @brief Destroy a compute pipeline.
        /// @param computePipeline Compute pipeline to destroy.
        void DestroyComputePipeline(GraphicsComputePipeline* computePipeline);

        /// @brief Destroy a swapchain.
        /// @param swapchain Swapchain to destroy.
        void DestroySwapchain(GraphicsSwapchain* swapchain);

        /// @brief Copy data to a target buffer.
        /// @param buffer Buffer to copy data to.
        /// @param data Pointer to the data to copy.
        /// @param size Size of the copy region.
        /// @param offset Offset into the target buffer.
        void CopyToBuffer(GraphicsBuffer* buffer, void const* data, size_t size, size_t offset);

        /// @brief Copy data from a target buffer.
        /// @param buffer Buffer to copy data from.
        /// @param outData Output data pointer to copy data to.
        /// @param size Size of the copy region.
        /// @param offset Offset into the target buffer.
        void CopyFromBuffer(GraphicsBuffer* buffer, void* outData, size_t size, size_t offset);

        /// @brief Start a new frame on the graphics device.
        /// @param swapchain Swapchains participating in this frame.
        /// @param swapchainCount Number of swapchains in the swapchains array.
        /// @return The frame context for the started frame.
        [[nodiscard]] FrameContext* StartFrame(GraphicsSwapchain** swapchains, size_t swapchainCount);

        /// @brief Begin a command buffer for recording rendering commands.
        /// @param frameContext Frame context to begin command buffer for.
        /// @return The command buffer.
        [[nodiscard]] GraphicsCommandBuffer* BeginGraphicsCommandBuffer(FrameContext* frameContext);

        /// @brief Submit a frame to the graphics device.
        /// @param frameContext Frame context to submit.
        void SubmitFrame(FrameContext* frameContext);

        /// @brief Start a new transfer batch on the graphics device.
        /// @return The new transfer batch.
        [[nodiscard]] TransferBatch* StartTransferBatch();
        
        /// @brief Begin a command buffer for recording transfer commands.
        /// @param transferBatch Transfer batch to begin command buffer for.
        /// @return The command buffer.
        [[nodiscard]] GraphicsCommandBuffer* BeginTransferCommandBuffer(TransferBatch* transferBatch);

        /// @brief Submit a transfer batch to the graphics device.
        /// @param transferBatch Transfer batch to submit.
        void SubmitTransferBatch(TransferBatch* transferBatch);

        /// @brief Wait for a transfer batch to complete.
        /// @param transferBatch Transfer batch to wait for.
        void WaitForTransferBatch(TransferBatch* transferBatch);

        /// @brief Get the active backbuffer texture for a swapchain.
        /// @param swapchain Swapchain to query. MUST be participating in the active frame.
        /// @return The graphics texture for the swapchain backbuffer.
        GraphicsTexture* GetSwapchainBackbuffer(GraphicsSwapchain* swapchain);

        /// @brief Get the active backbuffer texture view for a swapchain.
        /// @param swapchain Swapchain to query. MUST be participating in the active frame.
        /// @return The graphics texture view for the swapchain backbuffer.
        GraphicsTextureView* GetSwapchainBackbufferView(GraphicsSwapchain* swapchain);

        /// @brief Get the extent for a swapchain.
        /// @param swapchain Swapchain to query extent for.
        /// @return The swapchain extent.
        Extent2D GetSwapchainExtent(GraphicsSwapchain* swapchain);

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

#endif //BONSAI_GRAPHICS_DEVICE_HPP
