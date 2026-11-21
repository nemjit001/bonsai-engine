#include "VulkanObjectFactory.hpp"

#include <cassert>
#include <volk.h>
#include "Core/Logger.hpp"
#include "Gfx/Internal/GraphicsTypesInternal.hpp"
#include "Gfx/Vulkan/VulkanDescriptorSetAllocator.hpp"
#include "Gfx/Vulkan/VulkanGraphicsHelpers.hpp"
#include "Gfx/Vulkan/VulkanSwapchain.hpp"

namespace Bonsai
{
    VulkanObjectFactory::VulkanObjectFactory(
        VkInstance instance,
        VkPhysicalDevice physicalDevice,
        VkDevice device,
        VulkanSharedDeviceState* sharedDeviceState
    )
        :
        _instance(instance),
        _physicalDevice(physicalDevice),
        _device(device),
        _sharedDeviceState(sharedDeviceState)
    {
        //
    }

    VulkanObjectFactory::~VulkanObjectFactory()
    {
        //
    }

    VulkanObjectFactory* VulkanObjectFactory::TryCreate(
        VkInstance instance,
        VkPhysicalDevice physicalDevice,
        VkDevice device,
        VulkanSharedDeviceState* sharedDeviceState
    )
    {
        assert(instance != VK_NULL_HANDLE && "Vulkan instance was VK_NULL_HANDLE!");
        assert(physicalDevice != VK_NULL_HANDLE && "Vulkan physical device was VK_NULL_HANDLE!");
        assert(device != VK_NULL_HANDLE && "Vulkan device was VK_NULL_HANDLE!");
        assert(sharedDeviceState != nullptr && "Shared device state was null!");

        return new VulkanObjectFactory(instance, physicalDevice, device, sharedDeviceState);
    }
    
    VulkanBuffer* VulkanObjectFactory::CreateBuffer(GraphicsBufferDesc const& bufferDesc)
    {
        // Create vulkan buffer
        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.pNext = nullptr;
        bufferCreateInfo.flags = 0;
        bufferCreateInfo.size = static_cast<uint32_t>(bufferDesc.size);
        bufferCreateInfo.usage = VulkanGraphicsHelpers::GetVulkanBufferUsageFlags(bufferDesc.usage);
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferCreateInfo.queueFamilyIndexCount = 0;
        bufferCreateInfo.pQueueFamilyIndices = nullptr;

        VkBuffer buffer = VK_NULL_HANDLE;
        if (VK_CHECK_FAILED(vkCreateBuffer(_device, &bufferCreateInfo, nullptr, &buffer))) {
            return nullptr;
        }

        // Query buffer memory requirements
        VkMemoryRequirements memoryRequirements{};
        vkGetBufferMemoryRequirements(_device, buffer, &memoryRequirements);

        // Try to allocate memory
        std::optional<VkDeviceMemory> memory = TryAllocateMemory(bufferDesc.heapType, memoryRequirements);
        if (!memory.has_value())
        {
            vkDestroyBuffer(_device, buffer, nullptr);
            return nullptr;
        }

        // Bind buffer memory
        if (VK_CHECK_FAILED(vkBindBufferMemory(_device, buffer, memory.value(), 0)))
        {
            vkDestroyBuffer(_device, buffer, nullptr);
            vkFreeMemory(_device, memory.value(), nullptr);
            return nullptr;
        }

        // Map buffer memory if buffer is in a host visible heap
        void* mappedMemory = nullptr;
        if (bufferDesc.heapType == GraphicsHeapType_Upload || bufferDesc.heapType == GraphicsHeapType_ReadBack)
        {
            if (VK_CHECK_FAILED(vkMapMemory(_device, memory.value(), 0, bufferDesc.size, 0, &mappedMemory)))
            {
                vkDestroyBuffer(_device, buffer, nullptr);
                vkFreeMemory(_device, memory.value(), nullptr);
                return nullptr;
            }
        }

        // Create buffer and set mapped memory region
        auto* vulkanBuffer = new VulkanBuffer(_device, memory.value(), buffer);
        vulkanBuffer->SetDesc(bufferDesc);
        vulkanBuffer->SetMappedMemory(mappedMemory);
        return vulkanBuffer;
    }
    
    VulkanTexture* VulkanObjectFactory::CreateTexture(GraphicsTextureDesc const& textureDesc)
    {
        // Set texture depth and layers based on texture type
        uint32_t const depth = textureDesc.textureType == GraphicsTextureType_3D ? textureDesc.depthOrLayers : 1;
        uint32_t const layers = textureDesc.textureType != GraphicsTextureType_3D ? textureDesc.depthOrLayers : 1;

        // Create Vulkan image
        VkImageCreateInfo imageCreateInfo{};
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.pNext = nullptr;
        imageCreateInfo.flags = 0;
        imageCreateInfo.imageType = VulkanGraphicsHelpers::GetVulkanImageType(textureDesc.textureType);
        imageCreateInfo.format = VulkanGraphicsHelpers::GetVulkanFormat(textureDesc.textureFormat);
        imageCreateInfo.extent.width = static_cast<uint32_t>(textureDesc.width);
        imageCreateInfo.extent.height = static_cast<uint32_t>(textureDesc.height);
        imageCreateInfo.extent.depth = static_cast<uint32_t>(depth);
        imageCreateInfo.arrayLayers = static_cast<uint32_t>(layers);
        imageCreateInfo.mipLevels = static_cast<uint32_t>(textureDesc.mipLevels);
        imageCreateInfo.samples = VulkanGraphicsHelpers::GetVulkanSampleCountFlagBits(textureDesc.sampleCount);
        imageCreateInfo.tiling = VulkanGraphicsHelpers::GetVulkanImageTiling(textureDesc.tilingMode);
        imageCreateInfo.usage = VulkanGraphicsHelpers::GetVulkanImageUsageFlags(textureDesc.usage);
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageCreateInfo.queueFamilyIndexCount = 0;
        imageCreateInfo.pQueueFamilyIndices = nullptr;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VkImage image = VK_NULL_HANDLE;
        if (VK_CHECK_FAILED(vkCreateImage(_device, &imageCreateInfo, nullptr, &image))) {
            return nullptr;
        }

        // Query image memory requirements
        VkMemoryRequirements memoryRequirements{};
        vkGetImageMemoryRequirements(_device, image, &memoryRequirements);

        // Try to allocate memory
        std::optional<VkDeviceMemory> memory = TryAllocateMemory(textureDesc.heapType, memoryRequirements);
        if (!memory.has_value())
        {
            vkDestroyImage(_device, image, nullptr);
            return nullptr;
        }

        // Bind image memory
        if (VK_CHECK_FAILED(vkBindImageMemory(_device, image, memory.value(), 0)))
        {
            vkDestroyImage(_device, image, nullptr);
            vkFreeMemory(_device, memory.value(), nullptr);
            return nullptr;
        }

        auto* vulkanTexture = new VulkanTexture(_device, memory.value(), image);
        vulkanTexture->SetDesc(textureDesc);
        return vulkanTexture;
    }

    VulkanTextureView* VulkanObjectFactory::CreateTextureView(GraphicsTextureViewDesc const& textureViewDesc)
    {
        // Check if texture is valid
        auto* vulkanTexture = static_cast<VulkanTexture*>(textureViewDesc.texture);
        if (vulkanTexture == nullptr) {
            return nullptr;
        }

        // Create component mapping
        VkComponentMapping components{};
        components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        // Get actual layer and level counts
        uint32_t const levelCount = textureViewDesc.subresourceRange.levelCount == Graphics::RemainingMipLevels ?
            VK_REMAINING_MIP_LEVELS : textureViewDesc.subresourceRange.levelCount;
        uint32_t const layerCount = textureViewDesc.subresourceRange.layerCount == Graphics::RemainingArrayLayers ?
            VK_REMAINING_ARRAY_LAYERS : textureViewDesc.subresourceRange.layerCount;

        // Set up subresource range
        VkImageSubresourceRange subresourceRange{};
        subresourceRange.aspectMask = VulkanGraphicsHelpers::GetVulkanImageAspectFlags(textureViewDesc.subresourceRange.aspectMask);
        subresourceRange.baseMipLevel = textureViewDesc.subresourceRange.baseMipLevel;
        subresourceRange.levelCount = levelCount;
        subresourceRange.baseArrayLayer = textureViewDesc.subresourceRange.baseArrayLayer;
        subresourceRange.layerCount = layerCount;

        // Create image view
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.pNext = nullptr;
        imageViewCreateInfo.flags = 0;
        imageViewCreateInfo.image = vulkanTexture->GetImage();
        imageViewCreateInfo.format = VulkanGraphicsHelpers::GetVulkanFormat(textureViewDesc.viewFormat);
        imageViewCreateInfo.components = components;
        imageViewCreateInfo.subresourceRange = subresourceRange;

        VkImageView imageView = VK_NULL_HANDLE;
        if (VK_CHECK_FAILED(vkCreateImageView(_device, &imageViewCreateInfo, nullptr, &imageView))) {
            return nullptr;
        }

        auto* vulkanTextureView = new VulkanTextureView(_device, imageView);
        vulkanTextureView->SetDesc(textureViewDesc);
        vulkanTextureView->SetParentTexture(vulkanTexture);
        return vulkanTextureView;
    }

    VulkanSampler* VulkanObjectFactory::CreateSampler(GraphicsSamplerDesc const& samplerDesc)
    {
        // Create sampler
        VkSamplerCreateInfo samplerCreateInfo{};
        samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerCreateInfo.pNext = nullptr;
        samplerCreateInfo.flags = 0;
        samplerCreateInfo.magFilter = VulkanGraphicsHelpers::GetVulkanFilter(samplerDesc.magFilter);
        samplerCreateInfo.minFilter = VulkanGraphicsHelpers::GetVulkanFilter(samplerDesc.minFilter);
        samplerCreateInfo.mipmapMode = VulkanGraphicsHelpers::GetVulkanSamplerMipmapMode(samplerDesc.mipmapMode);
        samplerCreateInfo.addressModeU = VulkanGraphicsHelpers::GetVulkanSamplerAddressMode(samplerDesc.addressModeU);
        samplerCreateInfo.addressModeV = VulkanGraphicsHelpers::GetVulkanSamplerAddressMode(samplerDesc.addressModeV);
        samplerCreateInfo.addressModeW = VulkanGraphicsHelpers::GetVulkanSamplerAddressMode(samplerDesc.addressModeW);
        samplerCreateInfo.anisotropyEnable = samplerDesc.maxAnisotropy > 0 ? VK_TRUE : VK_FALSE;
        samplerCreateInfo.maxAnisotropy = static_cast<float>(samplerDesc.maxAnisotropy);
        samplerCreateInfo.compareEnable = samplerDesc.compareOp != GraphicsCompareOp_Undefined ? VK_TRUE : VK_FALSE;
        samplerCreateInfo.compareOp = VulkanGraphicsHelpers::GetVulkanCompareOp(samplerDesc.compareOp);
        samplerCreateInfo.minLod = samplerDesc.minLod;
        samplerCreateInfo.maxLod = samplerDesc.maxLod == Graphics::LODClampNone ? VK_LOD_CLAMP_NONE : samplerDesc.maxLod;
        samplerCreateInfo.borderColor = VulkanGraphicsHelpers::GetVulkanBorderColor(samplerDesc.borderColor);
        samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

        VkSampler sampler = VK_NULL_HANDLE;
        if (VK_CHECK_FAILED(vkCreateSampler(_device, &samplerCreateInfo, nullptr, &sampler))) {
            return nullptr;
        }

        auto* vulkanSampler = new VulkanSampler(_device, sampler);
        vulkanSampler->SetDesc(samplerDesc);
        return vulkanSampler;
    }

    VulkanDescriptorSetLayout* VulkanObjectFactory::CreateDescriptorSetLayout(GraphicsDescriptorSetLayoutDesc const& descriptorSetLayoutDesc)
    {
        // Set up descriptor bindings
        size_t const bindingCount = std::min(descriptorSetLayoutDesc.bindingCount, static_cast<size_t>(Graphics::MaxDescriptorSetBindings));
        std::array<VkDescriptorSetLayoutBinding, Graphics::MaxDescriptorSetBindings> bindings;
        for (size_t i = 0; i < bindingCount; i++)
        {
            GraphicsDescriptorSetLayoutBinding const& layoutBinding = descriptorSetLayoutDesc.bindings[i];
            VkDescriptorSetLayoutBinding& vulkanLayoutBinding = bindings[i];
            vulkanLayoutBinding.binding = layoutBinding.binding;
            vulkanLayoutBinding.descriptorType = VulkanGraphicsHelpers::GetVulkanDescriptorType(layoutBinding.descriptorType);
            vulkanLayoutBinding.descriptorCount = layoutBinding.descriptorCount;
            vulkanLayoutBinding.stageFlags = VulkanGraphicsHelpers::GetVulkanShaderStageFlags(layoutBinding.stages);
            vulkanLayoutBinding.pImmutableSamplers = nullptr; // TODO(nemjit001): Set up immutable samplers
        }

        // Create descriptor set layout
        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
        descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCreateInfo.pNext = nullptr;
        descriptorSetLayoutCreateInfo.flags = 0;
        descriptorSetLayoutCreateInfo.bindingCount = bindingCount;
        descriptorSetLayoutCreateInfo.pBindings = bindings.data();

        VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
        if (VK_CHECK_FAILED(vkCreateDescriptorSetLayout(_device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout))) {
            return nullptr;
        }

        return new VulkanDescriptorSetLayout(_device, descriptorSetLayout);
    }

    VulkanDescriptorSet* VulkanObjectFactory::CreateDescriptorSet(VulkanFrameContext* frameContext, GraphicsDescriptorSetDesc const& descriptorSetDesc)
    {
        auto* vulkanDescriptorSetLayout = static_cast<VulkanDescriptorSetLayout*>(descriptorSetDesc.descriptorSetLayout);

        // Allocate descriptor set from frame
        VulkanDescriptorSet* descriptorSet = frameContext->descriptorSetAllocator->AllocateDescriptorSet(vulkanDescriptorSetLayout);
        if (descriptorSet == nullptr) {
            return nullptr;
        }

        UpdateDescriptorSet(descriptorSet, descriptorSetDesc);
        return descriptorSet;
    }

    VulkanPipelineLayout* VulkanObjectFactory::CreatePipelineLayout(GraphicsPipelineLayoutDesc const& pipelineLayoutDesc)
    {
        // Get descriptor set layouts
        size_t const setLayoutCount = std::min(pipelineLayoutDesc.descriptorSetLayoutCount, static_cast<size_t>(Graphics::MaxBoundDescriptorSets));
        std::array<VkDescriptorSetLayout, Graphics::MaxBoundDescriptorSets> descriptorSetLayouts;
        for (size_t i = 0; i < setLayoutCount; i++)
        {
            auto* vulkanDescriptorSetLayout = static_cast<VulkanDescriptorSetLayout*>(pipelineLayoutDesc.descriptorSetLayouts[i]);
            descriptorSetLayouts[i] = vulkanDescriptorSetLayout->GetDescriptorSetLayout();
        }

        // Get push constant ranges
        size_t const pushConstantRangeCount = std::min(pipelineLayoutDesc.pushConstantRangeCount, static_cast<size_t>(Graphics::MaxPushConstantRanges));
        std::array<VkPushConstantRange, Graphics::MaxPushConstantRanges> pushConstantRanges;
        for (size_t i = 0; i < pushConstantRangeCount; i++)
        {
            GraphicsPushConstantRange const& pcRange = pipelineLayoutDesc.pushConstantRanges[i];
            VkPushConstantRange& vulkanPcRange = pushConstantRanges[i];
            vulkanPcRange.offset = pcRange.offset;
            vulkanPcRange.size = pcRange.size;
            vulkanPcRange.stageFlags = VulkanGraphicsHelpers::GetVulkanShaderStageFlags(pcRange.stages);
        }
        
        // Create pipeline layout
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.pNext = nullptr;
        pipelineLayoutCreateInfo.flags = 0;
        pipelineLayoutCreateInfo.setLayoutCount = setLayoutCount;
        pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutCreateInfo.pushConstantRangeCount = pushConstantRangeCount;
        pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();

        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
        if (VK_CHECK_FAILED(vkCreatePipelineLayout(_device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout))) {
            return nullptr;
        }

        return new VulkanPipelineLayout(_device, pipelineLayout);
    }

    VulkanRenderPipeline* VulkanObjectFactory::CreateRenderPipeline(GraphicsRenderPipelineDesc const& renderPipelineDesc)
    {
        /**
         * NOTE: this function is giant, but it cannot really be avoided since Vulkan pipeline setup just requires a lot of setup code.
         */

        // Check if pipeline layout is valid
        auto* vulkanPipelineLayout = static_cast<VulkanPipelineLayout*>(renderPipelineDesc.pipelineLayout);
        if (vulkanPipelineLayout == nullptr) {
            return nullptr;
        }

        // Compile pipeline
        std::optional<CompiledRenderPipelineStages> const compiledStages = TryCompileRenderPipeline(renderPipelineDesc);
        if (!compiledStages.has_value()) {
            return nullptr;
        }

        // Set up shader stages
        size_t stageCount = 0;
        std::array<VkPipelineShaderStageCreateInfo, 4> stages;
        if (compiledStages->vertexShader != VK_NULL_HANDLE) {
            VkPipelineShaderStageCreateInfo& vertexShaderStage = stages[stageCount];
            vertexShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vertexShaderStage.pNext = nullptr;
            vertexShaderStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
            vertexShaderStage.module = compiledStages->vertexShader;
            vertexShaderStage.pName = renderPipelineDesc.vertexShaderEntrypoint.name;
            vertexShaderStage.pSpecializationInfo = nullptr;

            stageCount++;
        }

        if (compiledStages->tessControlShader != VK_NULL_HANDLE) {
            VkPipelineShaderStageCreateInfo& tessControlShaderStage = stages[stageCount];
            tessControlShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            tessControlShaderStage.pNext = nullptr;
            tessControlShaderStage.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
            tessControlShaderStage.module = compiledStages->tessControlShader;
            tessControlShaderStage.pName = renderPipelineDesc.tessControlShaderEntrypoint.name;
            tessControlShaderStage.pSpecializationInfo = nullptr;

            stageCount++;
        }

        if (compiledStages->tessEvalShader != VK_NULL_HANDLE) {
            VkPipelineShaderStageCreateInfo& tessEvalShaderStage = stages[stageCount];
            tessEvalShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            tessEvalShaderStage.pNext = nullptr;
            tessEvalShaderStage.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
            tessEvalShaderStage.module = compiledStages->tessEvalShader;
            tessEvalShaderStage.pName = renderPipelineDesc.tessEvalShaderEntrypoint.name;
            tessEvalShaderStage.pSpecializationInfo = nullptr;

            stageCount++;
        }

        if (compiledStages->fragmentShader != VK_NULL_HANDLE) {
            VkPipelineShaderStageCreateInfo& fragmentShaderStage = stages[stageCount];
            fragmentShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            fragmentShaderStage.pNext = nullptr;
            fragmentShaderStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            fragmentShaderStage.module = compiledStages->fragmentShader;
            fragmentShaderStage.pName = renderPipelineDesc.fragmentShaderEntrypoint.name;
            fragmentShaderStage.pSpecializationInfo = nullptr;

            stageCount++;
        }

        // Set up vertex input state
        size_t vertexBindingCount = std::min(renderPipelineDesc.vertexInputState.vertexBindingCount, static_cast<size_t>(Graphics::MaxVertexInputBindings));
        size_t vertexAttributeCount = std::min(renderPipelineDesc.vertexInputState.vertexAttributeCount, static_cast<size_t>(Graphics::MaxVertexInputAttributes));

        std::array<VkVertexInputBindingDescription, Graphics::MaxVertexInputBindings> vertexBindingDescriptions;
        for (size_t i = 0; i < vertexBindingCount; i++)
        {
            GraphicsVertexBinding const& vertexBinding = renderPipelineDesc.vertexInputState.vertexBindings[i];
            VkVertexInputBindingDescription& vertexBindingDescription = vertexBindingDescriptions[i];
            vertexBindingDescription.binding = vertexBinding.binding;
            vertexBindingDescription.stride = vertexBinding.stride;
            vertexBindingDescription.inputRate = VulkanGraphicsHelpers::GetVulkanVertexInputRate(vertexBinding.inputRate);
        }

        std::array<VkVertexInputAttributeDescription, Graphics::MaxVertexInputAttributes> vertexAttributeDescriptions;
        for (size_t i = 0; i < vertexAttributeCount; i++)
        {
            GraphicsVertexAttribute const& vertexAttribute = renderPipelineDesc.vertexInputState.vertexAttributes[i];
            VkVertexInputAttributeDescription& vertexAttributeDescription = vertexAttributeDescriptions[i];
            vertexAttributeDescription.location = vertexAttribute.location;
            vertexAttributeDescription.binding = vertexAttribute.binding;
            vertexAttributeDescription.format = VulkanGraphicsHelpers::GetVulkanFormat(vertexAttribute.format);
            vertexAttributeDescription.offset = vertexAttribute.offset;
        }

        VkPipelineVertexInputStateCreateInfo vertexInputState{};
        vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputState.pNext = nullptr;
        vertexInputState.flags = 0;
        vertexInputState.vertexBindingDescriptionCount = vertexBindingCount;
        vertexInputState.pVertexBindingDescriptions = vertexBindingDescriptions.data();
        vertexInputState.vertexAttributeDescriptionCount = vertexAttributeCount;
        vertexInputState.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();

        // Set up input assembly state
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
        inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyState.pNext = nullptr;
        inputAssemblyState.flags = 0;
        inputAssemblyState.topology = VulkanGraphicsHelpers::GetVulkanPrimitiveTopology(renderPipelineDesc.primitiveTopology);
        inputAssemblyState.primitiveRestartEnable = renderPipelineDesc.stripCutValue != GraphicsIndexBufferStripCutValue_None ? VK_TRUE : VK_FALSE;

        // Set up tesselation state
        VkPipelineTessellationStateCreateInfo tessellationState{};
        tessellationState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        tessellationState.pNext = nullptr;
        tessellationState.flags = 0;
        tessellationState.patchControlPoints = 0;

        // Set up viewport state
        VkViewport const viewport{}; // Empty viewport
        VkRect2D const scissor{}; // Empty scissor rect
        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.pNext = nullptr;
        viewportState.flags = 0;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        // Set up rasterizer state
        VkPipelineRasterizationStateCreateInfo rasterizationState{};
        rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationState.pNext = nullptr;
        rasterizationState.flags = 0;
        rasterizationState.depthClampEnable = renderPipelineDesc.rasterState.depthClampEnable;
        rasterizationState.rasterizerDiscardEnable = VK_FALSE;
        rasterizationState.polygonMode = VulkanGraphicsHelpers::GetVulkanPolygonMode(renderPipelineDesc.rasterState.polygonMode);
        rasterizationState.cullMode = VulkanGraphicsHelpers::GetVulkanCullModeFlags(renderPipelineDesc.rasterState.cullMode);
        rasterizationState.frontFace = renderPipelineDesc.rasterState.frontCounterClockwise ? VK_FRONT_FACE_COUNTER_CLOCKWISE : VK_FRONT_FACE_CLOCKWISE;
        rasterizationState.depthBiasEnable = renderPipelineDesc.rasterState.depthBiasEnable;
        rasterizationState.depthBiasConstantFactor = renderPipelineDesc.rasterState.depthBiasConstantFactor;
        rasterizationState.depthBiasClamp = renderPipelineDesc.rasterState.depthBiasClamp;
        rasterizationState.depthBiasSlopeFactor = renderPipelineDesc.rasterState.depthBiasSlopeFactor;
        rasterizationState.lineWidth = 1.0F;

        // Set up multisample state
        VkPipelineMultisampleStateCreateInfo multisampleState{};
        multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleState.pNext = nullptr;
        multisampleState.flags = 0;
        multisampleState.rasterizationSamples = VulkanGraphicsHelpers::GetVulkanSampleCountFlagBits(renderPipelineDesc.multisampleState.sampleCount);
        multisampleState.sampleShadingEnable = VK_FALSE;
        multisampleState.minSampleShading = 0.0F;
        multisampleState.pSampleMask = renderPipelineDesc.multisampleState.sampleMask != 0 ? &renderPipelineDesc.multisampleState.sampleMask : nullptr;
        multisampleState.alphaToCoverageEnable = renderPipelineDesc.multisampleState.alphaToCoverage;
        multisampleState.alphaToOneEnable = renderPipelineDesc.multisampleState.alphaToOne;

        // Set up depth stencil state
        VkStencilOpState front{};
        front.failOp = VulkanGraphicsHelpers::GetVulkanStencilOp(renderPipelineDesc.depthStencilState.front.failOp);
        front.passOp = VulkanGraphicsHelpers::GetVulkanStencilOp(renderPipelineDesc.depthStencilState.front.passOp);
        front.depthFailOp = VulkanGraphicsHelpers::GetVulkanStencilOp(renderPipelineDesc.depthStencilState.front.depthFailOp);
        front.compareOp = VulkanGraphicsHelpers::GetVulkanCompareOp(renderPipelineDesc.depthStencilState.front.compareOp);
        front.compareMask = renderPipelineDesc.depthStencilState.stencilReadMask;
        front.writeMask = renderPipelineDesc.depthStencilState.stencilWriteMask;
        front.reference = 0;

        VkStencilOpState back{};
        back.failOp = VulkanGraphicsHelpers::GetVulkanStencilOp(renderPipelineDesc.depthStencilState.back.failOp);
        back.passOp = VulkanGraphicsHelpers::GetVulkanStencilOp(renderPipelineDesc.depthStencilState.back.passOp);
        back.depthFailOp = VulkanGraphicsHelpers::GetVulkanStencilOp(renderPipelineDesc.depthStencilState.back.depthFailOp);
        back.compareOp = VulkanGraphicsHelpers::GetVulkanCompareOp(renderPipelineDesc.depthStencilState.back.compareOp);
        back.compareMask = renderPipelineDesc.depthStencilState.stencilReadMask;
        back.writeMask = renderPipelineDesc.depthStencilState.stencilWriteMask;
        back.reference = 0;

        VkPipelineDepthStencilStateCreateInfo depthStencilState{};
        depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilState.pNext = nullptr;
        depthStencilState.flags = 0;
        depthStencilState.depthTestEnable = renderPipelineDesc.depthStencilState.depthTest;
        depthStencilState.depthWriteEnable = renderPipelineDesc.depthStencilState.depthWrite;
        depthStencilState.depthCompareOp = VulkanGraphicsHelpers::GetVulkanCompareOp(renderPipelineDesc.depthStencilState.depthCompareOp);
        depthStencilState.depthBoundsTestEnable = VK_FALSE;
        depthStencilState.stencilTestEnable = renderPipelineDesc.depthStencilState.stencilTest;
        depthStencilState.front = front;
        depthStencilState.back = back;
        depthStencilState.minDepthBounds = 0.0F;
        depthStencilState.maxDepthBounds = 0.0F;

        // Set up color blend state
        size_t const attachmentCount = std::min(renderPipelineDesc.renderTargetCount, static_cast<size_t>(Graphics::MaxRenderTargets));
        std::array<VkPipelineColorBlendAttachmentState, Graphics::MaxRenderTargets> colorBlendAttachments{};
        for (size_t i = 0; i < attachmentCount; i++)
        {
            GraphicsRenderTargetBlendState const& renderTargetBlendState = renderPipelineDesc.blendState.renderTargets[i];
            VkPipelineColorBlendAttachmentState& colorBlendAttachment = colorBlendAttachments[i];
            colorBlendAttachment.blendEnable = renderTargetBlendState.blendEnable;
            colorBlendAttachment.srcColorBlendFactor = VulkanGraphicsHelpers::GetVulkanBlendFactor(renderTargetBlendState.srcColorBlendFactor);
            colorBlendAttachment.dstColorBlendFactor = VulkanGraphicsHelpers::GetVulkanBlendFactor(renderTargetBlendState.dstColorBlendFactor);
            colorBlendAttachment.colorBlendOp = VulkanGraphicsHelpers::GetVulkanBlendOp(renderTargetBlendState.colorBlendOp);
            colorBlendAttachment.dstAlphaBlendFactor = VulkanGraphicsHelpers::GetVulkanBlendFactor(renderTargetBlendState.dstAlphaBlendFactor);
            colorBlendAttachment.dstAlphaBlendFactor = VulkanGraphicsHelpers::GetVulkanBlendFactor(renderTargetBlendState.dstAlphaBlendFactor);
            colorBlendAttachment.alphaBlendOp = VulkanGraphicsHelpers::GetVulkanBlendOp(renderTargetBlendState.alphaBlendOp);
            colorBlendAttachment.colorWriteMask = VulkanGraphicsHelpers::GetVulkanColorComponentFlags(renderTargetBlendState.colorWriteMask);
        }

        VkPipelineColorBlendStateCreateInfo colorBlendState{};
        colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendState.pNext = nullptr;
        colorBlendState.flags = 0;
        colorBlendState.logicOpEnable = renderPipelineDesc.blendState.logicOp != GraphicsLogicOp_Undefined ? VK_TRUE : VK_FALSE;
        colorBlendState.logicOp = VulkanGraphicsHelpers::GetVulkanLogicOp(renderPipelineDesc.blendState.logicOp);
        colorBlendState.attachmentCount = attachmentCount;
        colorBlendState.pAttachments = colorBlendAttachments.data();
        colorBlendState.blendConstants[0] = 0.0F;
        colorBlendState.blendConstants[1] = 0.0F;
        colorBlendState.blendConstants[2] = 0.0F;
        colorBlendState.blendConstants[3] = 0.0F;

        // Set up dynamic state
        constexpr VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
            VK_DYNAMIC_STATE_LINE_WIDTH,
            VK_DYNAMIC_STATE_BLEND_CONSTANTS,
            VK_DYNAMIC_STATE_STENCIL_REFERENCE,
        };

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.pNext = nullptr;
        dynamicState.flags = 0;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(std::size(dynamicStates));
        dynamicState.pDynamicStates = dynamicStates;

        // Set up rendering info
        size_t const colorAttachmentCount = std::min(renderPipelineDesc.renderTargetCount, static_cast<size_t>(Graphics::MaxRenderTargets));
        std::array<VkFormat, Graphics::MaxRenderTargets> colorAttachmentFormats;
        VkFormat const depthStencilFormat = VulkanGraphicsHelpers::GetVulkanFormat(renderPipelineDesc.depthStencilFormat);
        for (size_t i = 0; i < colorAttachmentCount; i++) {
            colorAttachmentFormats[i] = VulkanGraphicsHelpers::GetVulkanFormat(renderPipelineDesc.renderTargetFormats[i]);
        }

        VkPipelineRenderingCreateInfo renderingCreateInfo{};
        renderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        renderingCreateInfo.pNext = nullptr;
        renderingCreateInfo.viewMask = 0;
        renderingCreateInfo.colorAttachmentCount = colorAttachmentCount;
        renderingCreateInfo.pColorAttachmentFormats = colorAttachmentFormats.data();
        renderingCreateInfo.depthAttachmentFormat = GraphicsHelpers::IsDepthFormat(renderPipelineDesc.depthStencilFormat) ? depthStencilFormat : VK_FORMAT_UNDEFINED;
        renderingCreateInfo.stencilAttachmentFormat = GraphicsHelpers::IsStencilFormat(renderPipelineDesc.depthStencilFormat) ? depthStencilFormat : VK_FORMAT_UNDEFINED;;

        // Create render pipeline
        VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
        graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        graphicsPipelineCreateInfo.pNext = &renderingCreateInfo;
        graphicsPipelineCreateInfo.flags = 0;
        graphicsPipelineCreateInfo.stageCount = stageCount;
        graphicsPipelineCreateInfo.pStages = stages.data();
        graphicsPipelineCreateInfo.pVertexInputState = &vertexInputState;
        graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
        graphicsPipelineCreateInfo.pTessellationState = &tessellationState;
        graphicsPipelineCreateInfo.pViewportState = &viewportState;
        graphicsPipelineCreateInfo.pRasterizationState = &rasterizationState;
        graphicsPipelineCreateInfo.pMultisampleState = &multisampleState;
        graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilState;
        graphicsPipelineCreateInfo.pColorBlendState = &colorBlendState;
        graphicsPipelineCreateInfo.pDynamicState = &dynamicState;
        graphicsPipelineCreateInfo.layout = vulkanPipelineLayout->GetPipelineLayout();
        graphicsPipelineCreateInfo.renderPass = VK_NULL_HANDLE;
        graphicsPipelineCreateInfo.subpass = 0;
        graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        graphicsPipelineCreateInfo.basePipelineIndex = 0;

        VkPipeline graphicsPipeline = VK_NULL_HANDLE;
        if (VK_CHECK_FAILED(vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &graphicsPipeline)))
        {
            DestroyRenderPipelineStages(compiledStages.value());
            return nullptr;
        }

        DestroyRenderPipelineStages(compiledStages.value());
        return new VulkanRenderPipeline(_device, graphicsPipeline);
    }

    VulkanComputePipeline* VulkanObjectFactory::CreateComputePipeline(GraphicsComputePipelineDesc const& computePipelineDesc)
    {
        // Check if pipeline layout is valid
        auto* vulkanPipelineLayout = static_cast<VulkanPipelineLayout*>(computePipelineDesc.pipelineLayout);
        if (vulkanPipelineLayout == nullptr) {
            return nullptr;
        }

        // Compile pipeline
        std::optional<CompiledComputePipelineStages> const compiledStages = TryCompileComputePipeline(computePipelineDesc);
        if (!compiledStages.has_value()){
            return nullptr;
        }

        // Create compute pipeline
        VkPipelineShaderStageCreateInfo stage{};
        stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stage.pNext = nullptr;
        stage.flags = 0;
        stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        stage.module = compiledStages->computeShader;
        stage.pName = computePipelineDesc.computeShaderEntrypoint.name;
        stage.pSpecializationInfo = nullptr;

        VkComputePipelineCreateInfo computePipelineCreateInfo{};
        computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        computePipelineCreateInfo.pNext = nullptr;
        computePipelineCreateInfo.flags = 0;
        computePipelineCreateInfo.stage = stage;
        computePipelineCreateInfo.layout = vulkanPipelineLayout->GetPipelineLayout();
        computePipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        computePipelineCreateInfo.basePipelineIndex = 0;

        VkPipeline computePipeline = VK_NULL_HANDLE;
        if (VK_CHECK_FAILED(vkCreateComputePipelines(_device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &computePipeline)))
        {
            DestroyComputePipelineStages(compiledStages.value());
            return nullptr;
        }

        DestroyComputePipelineStages(compiledStages.value());
        return new VulkanComputePipeline(_device, computePipeline);
    }

    VulkanSwapchain* VulkanObjectFactory::CreateSwapchain(GraphicsSwapchainDesc const& swapchainDesc)
    {
        // Create swapchain
        VulkanSwapchain* swapchain = VulkanSwapchain::TryCreate(swapchainDesc.window, _instance, _physicalDevice, _device, _sharedDeviceState);
        if (swapchain == nullptr) {
            return nullptr;
        }
        
        // Configure swapchain
        VulkanSwapchainDesc vulkanSwapchainDesc{};
        vulkanSwapchainDesc.preferredFormat = swapchainDesc.preferredformat;
        vulkanSwapchainDesc.fallbackFormat = swapchainDesc.fallbackFormat;
        vulkanSwapchainDesc.width = swapchainDesc.width;
        vulkanSwapchainDesc.height = swapchainDesc.height;
        vulkanSwapchainDesc.syncInterval = swapchainDesc.syncInterval;
        vulkanSwapchainDesc.allowTearing = swapchainDesc.allowTearing;

        if (!swapchain->Configure(vulkanSwapchainDesc))
        {
            swapchain->ReleaseRef();
            return nullptr;
        }

        return swapchain;
    }

    std::optional<VkDeviceMemory> VulkanObjectFactory::TryAllocateMemory(GraphicsHeapType heapType, VkMemoryRequirements const& memoryRequirements)
    {
        // Get required memory property flags
        VkMemoryPropertyFlags const requiredMemoryProperties = VulkanGraphicsHelpers::GetVulkanMemoryPropertyFlags(heapType);
        if (requiredMemoryProperties == 0) {
            return {};
        }

        // Get memory type index
        uint32_t const memoryTypeIndex = VulkanGraphicsHelpers::FindMemoryTypeIndex(
            _sharedDeviceState->physicalDeviceMemoryProperties2.memoryProperties,
            memoryRequirements.memoryTypeBits,
            requiredMemoryProperties
        );
        if (memoryTypeIndex == UINT32_MAX) {
            return {};
        }

        // Allocate device memory
        VkMemoryAllocateInfo memoryAllocateInfo{};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.pNext = nullptr;
        memoryAllocateInfo.allocationSize = memoryRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

        VkDeviceMemory memory = VK_NULL_HANDLE;
        if (VK_CHECK_FAILED(vkAllocateMemory(_device, &memoryAllocateInfo, nullptr, &memory))) {
            return {};
        }

        return memory;
    }

    void VulkanObjectFactory::UpdateDescriptorSet(VulkanDescriptorSet const* descriptorSet, GraphicsDescriptorSetDesc const& descriptorSetDesc)
    {
        // Cannot write to an invalid descriptor set
        if (descriptorSet == nullptr) {
            return;
        }

        // Set up descriptor set write
        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.pNext = nullptr;
        descriptorWrite.dstSet = descriptorSet->GetDescriptorSet();
        descriptorWrite.dstBinding = descriptorSetDesc.binding;
        descriptorWrite.dstArrayElement = descriptorSetDesc.arrayElement;
        descriptorWrite.descriptorCount = descriptorSetDesc.descriptorCount;
        descriptorWrite.descriptorType = VulkanGraphicsHelpers::GetVulkanDescriptorType(descriptorSetDesc.descriptorType);
        descriptorWrite.pBufferInfo = nullptr;
        descriptorWrite.pImageInfo = nullptr;
        descriptorWrite.pTexelBufferView = nullptr;

        // Set up optional buffer infos
        std::vector<VkDescriptorBufferInfo> bufferInfos;
        if (descriptorSetDesc.bufferInfos != nullptr)
        {
            bufferInfos.resize(descriptorSetDesc.descriptorCount);
            for (size_t i = 0; i < descriptorSetDesc.descriptorCount; i++)
            {
                GraphicsDescriptorBufferInfo const& bufferInfo = descriptorSetDesc.bufferInfos[i];
                auto* vulkanBuffer = static_cast<VulkanBuffer*>(bufferInfo.buffer);

                VkDescriptorBufferInfo& vulkanBufferInfo = bufferInfos[i];
                vulkanBufferInfo.buffer = vulkanBuffer->GetBuffer();
                vulkanBufferInfo.offset = bufferInfo.offset;
                vulkanBufferInfo.range = bufferInfo.range == Graphics::WholeSize ? VK_WHOLE_SIZE : bufferInfo.range;
            }

            descriptorWrite.pBufferInfo = bufferInfos.data();
        }

        // Set up optional image infos
        std::vector<VkDescriptorImageInfo> imageInfos;
        if (descriptorSetDesc.imageInfos != nullptr)
        {
            imageInfos.resize(descriptorSetDesc.descriptorCount);
            for (size_t i = 0; i < descriptorSetDesc.descriptorCount; i++)
            {
                GraphicsDescriptorImageInfo const& imageInfo = descriptorSetDesc.imageInfos[i];
                auto* vulkanSampler = static_cast<VulkanSampler*>(imageInfo.sampler);
                auto* vulkanTextureView = static_cast<VulkanTextureView*>(imageInfo.textureView);

                VkDescriptorImageInfo& vulkanImageInfo = imageInfos[i];
                vulkanImageInfo.sampler = vulkanSampler != nullptr ? vulkanSampler->GetSampler() : nullptr;
                vulkanImageInfo.imageView = vulkanTextureView != nullptr ? vulkanTextureView->GetImageView() : nullptr;
                vulkanImageInfo.imageLayout = VulkanGraphicsHelpers::GetVulkanImageLayout(imageInfo.textureLayout);
            }

            descriptorWrite.pImageInfo = imageInfos.data();
        }

        // Update descriptor set
        vkUpdateDescriptorSets(_device, 1, &descriptorWrite, 0, nullptr);
    }

    std::optional<VulkanObjectFactory::CompiledRenderPipelineStages> VulkanObjectFactory::TryCompileRenderPipeline(GraphicsRenderPipelineDesc const& renderPipelineDesc)
    {
        // Compile vertex shader
        VkShaderModule vertexShaderModule = VK_NULL_HANDLE;
        if (renderPipelineDesc.vertexShaderBytecode.code != nullptr
            && renderPipelineDesc.vertexShaderBytecode.size != 0)
        {
            VkShaderModuleCreateInfo vertexShaderModuleCreateInfo{};
            vertexShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            vertexShaderModuleCreateInfo.pNext = nullptr;
            vertexShaderModuleCreateInfo.flags = 0;
            vertexShaderModuleCreateInfo.pCode = reinterpret_cast<uint32_t const*>(renderPipelineDesc.vertexShaderBytecode.code);
            vertexShaderModuleCreateInfo.codeSize = renderPipelineDesc.vertexShaderBytecode.size;

            if (VK_CHECK_FAILED(vkCreateShaderModule(_device, &vertexShaderModuleCreateInfo, nullptr, &vertexShaderModule))) {
                vertexShaderModule = VK_NULL_HANDLE;
            }
        }

        // Compile tesselation control shader
        VkShaderModule tessControlShaderModule = VK_NULL_HANDLE;
        if (renderPipelineDesc.tessControlShaderBytecode.code != nullptr
            && renderPipelineDesc.tessControlShaderBytecode.size != 0)
        {
            VkShaderModuleCreateInfo tessControlShaderModuleCreateInfo{};
            tessControlShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            tessControlShaderModuleCreateInfo.pNext = nullptr;
            tessControlShaderModuleCreateInfo.flags = 0;
            tessControlShaderModuleCreateInfo.pCode = reinterpret_cast<uint32_t const*>(renderPipelineDesc.tessControlShaderBytecode.code);
            tessControlShaderModuleCreateInfo.codeSize = renderPipelineDesc.tessControlShaderBytecode.size;

            if (VK_CHECK_FAILED(vkCreateShaderModule(_device, &tessControlShaderModuleCreateInfo, nullptr, &tessControlShaderModule))) {
                tessControlShaderModule = VK_NULL_HANDLE;
            }
        }

        // Compile tesselation evaluation shader
        VkShaderModule tessEvalShaderModule = VK_NULL_HANDLE;
        if (renderPipelineDesc.tessEvalShaderBytecode.code != nullptr
            && renderPipelineDesc.tessEvalShaderBytecode.size != 0)
        {
            VkShaderModuleCreateInfo tessEvalShaderModuleCreateInfo{};
            tessEvalShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            tessEvalShaderModuleCreateInfo.pNext = nullptr;
            tessEvalShaderModuleCreateInfo.flags = 0;
            tessEvalShaderModuleCreateInfo.pCode = reinterpret_cast<uint32_t const*>(renderPipelineDesc.tessEvalShaderBytecode.code);
            tessEvalShaderModuleCreateInfo.codeSize = renderPipelineDesc.tessEvalShaderBytecode.size;

            if (VK_CHECK_FAILED(vkCreateShaderModule(_device, &tessEvalShaderModuleCreateInfo, nullptr, &tessEvalShaderModule))) {
                tessEvalShaderModule = VK_NULL_HANDLE;
            }
        }

        // Compile fragment shader
        VkShaderModule fragmentShaderModule = VK_NULL_HANDLE;
        if (renderPipelineDesc.fragmentShaderBytecode.code != nullptr
            && renderPipelineDesc.fragmentShaderBytecode.size != 0)
        {
            VkShaderModuleCreateInfo fragmentShaderModuleCreateInfo{};
            fragmentShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            fragmentShaderModuleCreateInfo.pNext = nullptr;
            fragmentShaderModuleCreateInfo.flags = 0;
            fragmentShaderModuleCreateInfo.pCode = reinterpret_cast<uint32_t const*>(renderPipelineDesc.fragmentShaderBytecode.code);
            fragmentShaderModuleCreateInfo.codeSize = renderPipelineDesc.fragmentShaderBytecode.size;

            if (VK_CHECK_FAILED(vkCreateShaderModule(_device, &fragmentShaderModuleCreateInfo, nullptr, &fragmentShaderModule))) {
                fragmentShaderModule = VK_NULL_HANDLE;
            }
        }

        return CompiledRenderPipelineStages{
            vertexShaderModule,
            tessControlShaderModule,
            tessEvalShaderModule,
            fragmentShaderModule,
        };
    }

    void VulkanObjectFactory::DestroyRenderPipelineStages(CompiledRenderPipelineStages const& renderPipelineStages)
    {
        vkDestroyShaderModule(_device, renderPipelineStages.vertexShader, nullptr);
        vkDestroyShaderModule(_device, renderPipelineStages.tessControlShader, nullptr);
        vkDestroyShaderModule(_device, renderPipelineStages.tessEvalShader, nullptr);
        vkDestroyShaderModule(_device, renderPipelineStages.fragmentShader, nullptr);
    }

    std::optional<VulkanObjectFactory::CompiledComputePipelineStages> VulkanObjectFactory::TryCompileComputePipeline(GraphicsComputePipelineDesc const& computePipelineDesc)
    {
        VkShaderModule computeShaderModule = VK_NULL_HANDLE;
        if (computePipelineDesc.computeShaderBytecode.code != nullptr
            && computePipelineDesc.computeShaderBytecode.size != 0)
        {
            VkShaderModuleCreateInfo computeShaderModuleCreateInfo{};
            computeShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            computeShaderModuleCreateInfo.pNext = nullptr;
            computeShaderModuleCreateInfo.flags = 0;
            computeShaderModuleCreateInfo.pCode = reinterpret_cast<uint32_t const*>(computePipelineDesc.computeShaderBytecode.code);
            computeShaderModuleCreateInfo.codeSize = computePipelineDesc.computeShaderBytecode.size;

            if (VK_CHECK_FAILED(vkCreateShaderModule(_device, &computeShaderModuleCreateInfo, nullptr, &computeShaderModule))) {
                return {};
            }
        }

        return CompiledComputePipelineStages{
            computeShaderModule,
        };
    }

    void VulkanObjectFactory::DestroyComputePipelineStages(CompiledComputePipelineStages const& computePipelineStages)
    {
        vkDestroyShaderModule(_device, computePipelineStages.computeShader, nullptr);
    }
} // namespace Bonsai
