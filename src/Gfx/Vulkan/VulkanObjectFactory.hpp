#pragma once
#ifndef BONSAI_VULKAN_OBJECT_FACTORY
#define BONSAI_VULKAN_OBJECT_FACTORY

#define VK_NO_PROTOTYPES

#include <optional>
#include <vulkan/vulkan.h>
#include "Gfx/Vulkan/VulkanGraphicsTypes.hpp"

namespace Bonsai
{
    /// @brief Factory for creating Vulkan graphics objects.
    class VulkanObjectFactory
    {
    private:
        VulkanObjectFactory(
            VkInstance instance,
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            VulkanSharedDeviceState* sharedDeviceState
        );

    public:
        ~VulkanObjectFactory();

        VulkanObjectFactory(VulkanObjectFactory const&) = delete;
        VulkanObjectFactory& operator=(VulkanObjectFactory const&) = delete;

        /// @brief Try to create a Vulkan object factory.
        /// @param instance Instance to use in the object factory.
        /// @param physicalDevice Physical device to use in the object factory.
        /// @param device Logical device to use in the object factory.
        /// @param sharedDeviceState Shared device state.
        /// @return 
        [[nodiscard]] static VulkanObjectFactory* TryCreate(
            VkInstance instance,
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            VulkanSharedDeviceState* sharedDeviceState
        );

        /// @brief Create a Vulkan buffer.
        /// @param bufferDesc Buffer description to use for creation.
        /// @return A new Vulkan buffer.
        VulkanBuffer* CreateBuffer(GraphicsBufferDesc const& bufferDesc);

        /// @brief Create a Vulkan texture.
        /// @param textureDesc Texture description to use for creation.
        /// @return A new Vulkan texture.
        VulkanTexture* CreateTexture(GraphicsTextureDesc const& textureDesc);

        /// @brief Create a Vulkan texture view.
        /// @param textureViewDesc Texture view description to use for creation.
        /// @return A new Vulkan texture view.
        VulkanTextureView* CreateTextureView(GraphicsTextureViewDesc const& textureViewDesc);

        /// @brief Create a Vulkan sampler.
        /// @param samplerDesc Sampler description to use for creation.
        /// @return A new Vulkan sampler.
        VulkanSampler* CreateSampler(GraphicsSamplerDesc const& samplerDesc);
        
        /// @brief Create a Vulkan descriptor set layout.
        /// @param descriptorSetLayoutDesc Descriptor set layout description to use for creation.
        /// @return A new Vulkan descriptor set layout.
        VulkanDescriptorSetLayout* CreateDescriptorSetLayout(GraphicsDescriptorSetLayoutDesc const& descriptorSetLayoutDesc);

        /// @brief Create a Vulkan descriptor set for use in a given frame.
        /// @param frameContext Frame to allocated Vulkan descriptor set for.
        /// @param descriptorSetDesc Descriptor set description to use for creation.
        /// @return A new Vulkan descriptor set.
        VulkanDescriptorSet* CreateDescriptorSet(VulkanFrameContext* frameContext, GraphicsDescriptorSetDesc const& descriptorSetDesc);

        /// @brief Create a Vulkan pipeline layout.
        /// @param pipelineLayoutDesc Pipeline layout description to use for creation.
        /// @return A new Vulkan pipeline layout.
        VulkanPipelineLayout* CreatePipelineLayout(GraphicsPipelineLayoutDesc const& pipelineLayoutDesc);

        /// @brief Create a Vulkan render pipeline.
        /// @param renderPipelineDesc Render pipeline description to use for creation.
        /// @return A new Vulkan render pipeline.
        VulkanRenderPipeline* CreateRenderPipeline(GraphicsRenderPipelineDesc const& renderPipelineDesc);

        /// @brief Create a Vulkan compute pipeline.
        /// @param computePipelineDesc Compute pipeline description to use for creation.
        /// @return A new Vulkan compute pipeline.
        VulkanComputePipeline* CreateComputePipeline(GraphicsComputePipelineDesc const& computePipelineDesc);

        /// @brief Create a Vulkan swapchain.
        /// @param swapchainDesc Swapchain description to use for creation.
        /// @return A new Vulkan swapchain.
        VulkanSwapchain* CreateSwapchain(GraphicsSwapchainDesc const& swapchainDesc);

    private:
        /// @brief Compiled pipeline stages for render pipelines.
        struct CompiledRenderPipelineStages
        {
            VkShaderModule vertexShader;
            VkShaderModule tessControlShader;
            VkShaderModule tessEvalShader;
            VkShaderModule fragmentShader;
        };

        /// @brief Compiled pipeline stages for compute pipelines.
        struct CompiledComputePipelineStages
        {
            VkShaderModule computeShader;
        };

        /// @brief Attempt to allocate device memory.
        /// @param heapType Heap type to allocate memory from.
        /// @param memoryRequirements Memory requirements as queried from the resource.
        /// @return An optional Vulkan device memory allocation.
        std::optional<VkDeviceMemory> TryAllocateMemory(GraphicsHeapType heapType, VkMemoryRequirements const& memoryRequirements);

        /// @brief Update a descriptor set based on a descriptor set description.
        /// @param descriptorSet Descriptor set to update.
        /// @param descriptorSetDesc Descriptor set description to use for update.
        void UpdateDescriptorSet(VulkanDescriptorSet const* descriptorSet, GraphicsDescriptorSetDesc const& descriptorSetDesc);

        /// @brief Try to compile a render pipeline.
        /// @param renderPipelineDesc Render pipeline description to compile shaders for.
        /// @return An optional compiled render pipeline.
        std::optional<CompiledRenderPipelineStages> TryCompileRenderPipeline(GraphicsRenderPipelineDesc const& renderPipelineDesc);

        /// @brief Destroy compiled render pipeline stages.
        /// @param renderPipelineStages Render pipeline stages to destroy.
        void DestroyRenderPipelineStages(CompiledRenderPipelineStages const& renderPipelineStages);
        
        /// @brief Try to compile a compute pipeline.
        /// @param computePipelineDesc Compute pipeline description to compiled shaders for.
        /// @return An optional compiled compute pipeline.
        std::optional<CompiledComputePipelineStages> TryCompileComputePipeline(GraphicsComputePipelineDesc const& computePipelineDesc);

        /// @brief Destroy compiled compute pipeline stages.
        /// @param computePipelineStages Compute pipeline stages to destroy.
        void DestroyComputePipelineStages(CompiledComputePipelineStages const& computePipelineStages);

    private:
        VkInstance _instance;
        VkPhysicalDevice _physicalDevice;
        VkDevice _device;
        VulkanSharedDeviceState* _sharedDeviceState;
    };
} // namespace Bonsai

#endif //BONSAI_VULKAN_OBJECT_FACTORY
