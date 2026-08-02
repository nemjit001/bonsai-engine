#include "WebGPUGraphicsDevice.hpp"
#if BONSAI_RHI_WEBGPU_SUPPORT

#include <Containers/StaticArray.hpp>
#include "WebGPUTypes.hpp"

namespace RHI
{
    /// @brief Texture dims LUT.
    static constexpr WGPUTextureDimension gTextureDimensionLUT[] = {
        WGPUTextureDimension_1D,
        WGPUTextureDimension_2D,
        WGPUTextureDimension_3D,
    };

    /// @brief Texture view dims LUT.
    static constexpr WGPUTextureViewDimension gTextureViewDimensionLUT[] = {
        WGPUTextureViewDimension_1D,
        WGPUTextureViewDimension_2D,
        WGPUTextureViewDimension_3D,
        WGPUTextureViewDimension_Cube,
        WGPUTextureViewDimension_Undefined, // View dimension 1D array unsupported
        WGPUTextureViewDimension_2DArray,
        WGPUTextureViewDimension_CubeArray,
    };

    /// @brief Texture format LUT.
    static constexpr WGPUTextureFormat gTextureFormatLUT[] = {
        WGPUTextureFormat_Undefined,
        //
        WGPUTextureFormat_R8Unorm,
        WGPUTextureFormat_R8Snorm,
        WGPUTextureFormat_R8Uint,
        WGPUTextureFormat_R8Sint,
        //
        WGPUTextureFormat_RG8Unorm,
        WGPUTextureFormat_RG8Snorm,
        WGPUTextureFormat_RG8Uint,
        WGPUTextureFormat_RG8Sint,
        //
        WGPUTextureFormat_RGBA8Unorm,
        WGPUTextureFormat_RGBA8UnormSrgb,
        WGPUTextureFormat_RGBA8Snorm,
        WGPUTextureFormat_RGBA8Uint,
        WGPUTextureFormat_RGBA8Sint,
        //
        WGPUTextureFormat_BGRA8Unorm,
        WGPUTextureFormat_BGRA8UnormSrgb,
        //
        WGPUTextureFormat_R16Uint,
        WGPUTextureFormat_R16Sint,
        WGPUTextureFormat_R16Float,
        //
        WGPUTextureFormat_RG16Uint,
        WGPUTextureFormat_RG16Sint,
        WGPUTextureFormat_RG16Float,
        //
        WGPUTextureFormat_RGBA16Uint,
        WGPUTextureFormat_RGBA16Sint,
        WGPUTextureFormat_RGBA16Float,
        //
        WGPUTextureFormat_R32Uint,
        WGPUTextureFormat_R32Sint,
        WGPUTextureFormat_R32Float,
        //
        WGPUTextureFormat_RG32Uint,
        WGPUTextureFormat_RG32Sint,
        WGPUTextureFormat_RG32Float,
        //
        WGPUTextureFormat_RGBA32Uint,
        WGPUTextureFormat_RGBA32Sint,
        WGPUTextureFormat_RGBA32Float,
        //
        WGPUTextureFormat_Stencil8,
        WGPUTextureFormat_Depth16Unorm,
        WGPUTextureFormat_Depth24Plus,
        WGPUTextureFormat_Depth24PlusStencil8,
        WGPUTextureFormat_Depth32Float,
        WGPUTextureFormat_Depth32FloatStencil8,
    };

    /// @brief Filter mode LUT.
    static constexpr WGPUFilterMode gFilterModeLUT[] = {
        WGPUFilterMode_Nearest,
        WGPUFilterMode_Linear,
    };

    /// @brief Mipmap filter mode LUT.
    static constexpr WGPUMipmapFilterMode gMipmapFilterModeLUT[] = {
        WGPUMipmapFilterMode_Nearest,
        WGPUMipmapFilterMode_Linear,
    };

    /// @brief Address mode LUT.
    static constexpr WGPUAddressMode gAddressModeLUT[] = {
        WGPUAddressMode_Repeat,
        WGPUAddressMode_MirrorRepeat,
        WGPUAddressMode_ClampToEdge,
        WGPUAddressMode_ClampToEdge, // Address mode clamp to border unsupported
    };

    /// @brief Buffer binding type LUT for translating descriptor type.
    static constexpr WGPUBufferBindingType gBufferBindingTypeLUT[] = {
        WGPUBufferBindingType_Undefined, // Not buffer descriptor types
        WGPUBufferBindingType_Undefined, // Not buffer descriptor types
        WGPUBufferBindingType_Undefined, // Not buffer descriptor types
        WGPUBufferBindingType_Undefined, // Not buffer descriptor types
        WGPUBufferBindingType_Uniform,
        WGPUBufferBindingType_Storage,
        WGPUBufferBindingType_Uniform,
        WGPUBufferBindingType_Storage,
    };

    /// @brief Cull mode LUT.
    static constexpr WGPUCullMode gCullModeLUT[] = {
        WGPUCullMode_None,
        WGPUCullMode_Front,
        WGPUCullMode_Back,
    };

    /// @brief Primitive topology LUT.
    static constexpr WGPUPrimitiveTopology gPrimitiveTopologyLUT[] = {
        WGPUPrimitiveTopology_PointList,
        WGPUPrimitiveTopology_LineList,
        WGPUPrimitiveTopology_LineStrip,
        WGPUPrimitiveTopology_TriangleList,
        WGPUPrimitiveTopology_TriangleStrip,
        WGPUPrimitiveTopology_Undefined, // Triangle fan unsupported
    };

    /// @brief Compare function LUT.
    static constexpr WGPUCompareFunction gCompareFunctionLUT[] = {
        WGPUCompareFunction_Never,
        WGPUCompareFunction_Less,
        WGPUCompareFunction_Equal,
        WGPUCompareFunction_LessEqual,
        WGPUCompareFunction_Greater,
        WGPUCompareFunction_NotEqual,
        WGPUCompareFunction_GreaterEqual,
        WGPUCompareFunction_Always,
    };

    /// @brief Blend operation LUT.
    static constexpr WGPUBlendOperation gBlendOperationLUT[] = {
        WGPUBlendOperation_Add,
        WGPUBlendOperation_Subtract,
        WGPUBlendOperation_ReverseSubtract,
        WGPUBlendOperation_Min,
        WGPUBlendOperation_Max,
    };

    /// @brief Load op LUT.
    static constexpr WGPULoadOp gLoadOpLUT[] = {
        WGPULoadOp_Load,
        WGPULoadOp_Clear,
        WGPULoadOp_Undefined,
    };

    /// @brief Store op LUT.
    static constexpr WGPUStoreOp gStoreOpLUT[] = {
        WGPUStoreOp_Store,
        WGPUStoreOp_Undefined,
    };

    /// @brief Blend factor LUT.
    static constexpr WGPUBlendFactor gBlendFactorLUT[] = {
        WGPUBlendFactor_Zero,
        WGPUBlendFactor_One,
        WGPUBlendFactor_Src,
        WGPUBlendFactor_Dst,
        WGPUBlendFactor_SrcAlpha,
        WGPUBlendFactor_DstAlpha,
        WGPUBlendFactor_Constant,
        WGPUBlendFactor_Constant,
        WGPUBlendFactor_SrcAlphaSaturated,
        WGPUBlendFactor_Src1,
        WGPUBlendFactor_Src1Alpha,
        WGPUBlendFactor_OneMinusSrc,
        WGPUBlendFactor_OneMinusDst,
        WGPUBlendFactor_OneMinusSrcAlpha,
        WGPUBlendFactor_OneMinusDstAlpha,
        WGPUBlendFactor_OneMinusConstant,
        WGPUBlendFactor_OneMinusConstant,
        WGPUBlendFactor_OneMinusSrc1,
        WGPUBlendFactor_OneMinusSrc1Alpha,
    };

    /// @brief Get the WebGPU buffer usage flags.
    /// @param usageFlags 
    /// @return 
    static WGPUBufferUsage GetBufferUsage(RHI::BufferUsageFlags usageFlags)
    {
        WGPUBufferUsage usage = 0;
        if (BitFlagIsSet(usageFlags, BufferUsage_TransferSrc)) {
            usage |= WGPUBufferUsage_CopySrc;
        }
        if (BitFlagIsSet(usageFlags, BufferUsage_TransferDst)) {
            usage |= WGPUBufferUsage_CopyDst;
        }
        if (BitFlagIsSet(usageFlags, BufferUsage_UniformBuffer)) {
            usage |= WGPUBufferUsage_Uniform;
        }
        if (BitFlagIsSet(usageFlags, BufferUsage_StorageBuffer)) {
            usage |= WGPUBufferUsage_Storage;
        }
        if (BitFlagIsSet(usageFlags, BufferUsage_IndexBuffer)) {
            usage |= WGPUBufferUsage_Index;
        }
        if (BitFlagIsSet(usageFlags, BufferUsage_VertexBuffer)) {
            usage |= WGPUBufferUsage_Vertex;
        }
        if (BitFlagIsSet(usageFlags, BufferUsage_IndirectBuffer)) {
            usage |= WGPUBufferUsage_Indirect;
        }

        return usage;
    };

    /// @brief Get the WebGPU texture usage flags.
    /// @param usageFlags 
    /// @return 
    static WGPUTextureUsage GetTextureUsage(TextureUsageFlags usageFlags)
    {
        WGPUTextureUsage usage = 0;
        if (BitFlagIsSet(usageFlags, TextureUsage_TransferSrc)) {
            usage |= WGPUTextureUsage_CopySrc;
        }
        if (BitFlagIsSet(usageFlags, TextureUsage_TransferDst)) {
            usage |= WGPUTextureUsage_CopyDst;
        }
        if (BitFlagIsSet(usageFlags, TextureUsage_Sampled)) {
            usage |= WGPUTextureUsage_TextureBinding;
        }
        if (BitFlagIsSet(usageFlags, TextureUsage_Storage)) {
            usage |= WGPUTextureUsage_StorageBinding;
        }
        if (BitFlagIsSet(usageFlags, TextureUsage_RenderTarget)
            || BitFlagIsSet(usageFlags, TextureUsage_DepthStencilTarget)) {
            usage |= WGPUTextureUsage_RenderAttachment;
        }

        return usage;
    }

    /// @brief Get the WebGPU shader stage flags.
    /// @param stageFlags 
    /// @return 
    static WGPUShaderStage GetShaderStage(ShaderStageFlags stageFlags)
    {
        WGPUShaderStage stages = 0;
        if (BitFlagIsSet(stageFlags, ShaderStage_Vertex)) {
            stages |= WGPUShaderStage_Vertex;
        }
        if (BitFlagIsSet(stageFlags, ShaderStage_Pixel)) {
            stages |= WGPUShaderStage_Fragment;
        }
        if (BitFlagIsSet(stageFlags, ShaderStage_Compute)) {
            stages |= WGPUShaderStage_Compute;
        }

        return stages;
    }

    /// @brief Get the WebGPU color write mask.
    /// @param colorComponentFlags 
    /// @return 
    static WGPUColorWriteMask GetColorWriteMask(ColorComponentFlags colorComponentFlags)
    {
        WGPUColorWriteMask mask = 0;
        if (BitFlagIsSet(colorComponentFlags, ColorComponent_Red)) {
            mask |= WGPUColorWriteMask_Red;
        }
        if (BitFlagIsSet(colorComponentFlags, ColorComponent_Green)) {
            mask |= WGPUColorWriteMask_Green;
        }
        if (BitFlagIsSet(colorComponentFlags, ColorComponent_Blue)) {
            mask |= WGPUColorWriteMask_Blue;
        }
        if (BitFlagIsSet(colorComponentFlags, ColorComponent_Alpha)) {
            mask |= WGPUColorWriteMask_Alpha;
        }
        if (BitFlagIsSet(colorComponentFlags, ColorComponent_All)) {
            mask |= WGPUColorWriteMask_All;
        }

        return mask;
    }

    /// @brief Get a WebGPU texture aspect mask.
    /// @param aspectMask 
    /// @return 
    static WGPUTextureAspect GetTextureAspect(TextureAspectMask aspectMask)
    {
        WGPUTextureAspect aspect = WGPUTextureAspect_Undefined;
        if (BitFlagIsSet(aspectMask, TextureAspect_Color) || (BitFlagIsSet(aspectMask, TextureAspect_Depth) && BitFlagIsSet(aspectMask, TextureAspect_Stencil))) {
            aspect = WGPUTextureAspect_All;
        }
        else if (BitFlagIsSet(aspectMask, TextureAspect_Depth)) {
            aspect = WGPUTextureAspect_DepthOnly;
        }
        else if (BitFlagIsSet(aspectMask, TextureAspect_Stencil)) {
            aspect = WGPUTextureAspect_StencilOnly;
        }

        return aspect;
    }

    WebGPUCommandBuffer::WebGPUCommandBuffer(WGPUCommandEncoder commandEncoder)
        :
        _commandEncoder(commandEncoder)
    {
        //
    }

    WebGPUCommandBuffer::~WebGPUCommandBuffer()
    {
        if (_commandEncoder) {
            wgpuCommandEncoderRelease(_commandEncoder);
        }

        if (_commandBuffer) {
            wgpuCommandBufferRelease(_commandBuffer);
        }

        if (_activeRenderPass) {
            wgpuRenderPassEncoderRelease(_activeRenderPass);
        }

        if (_activeComputePass) {
            wgpuComputePassEncoderRelease(_activeComputePass);
        }
    }

    void WebGPUCommandBuffer::CopyBufferToBuffer(GPUBuffer* src, GPUBuffer* dest, uint64 size, int64 srcOffset, int64 destOffset)
    {
        BONSAI_ASSERT(_activeRenderPass == nullptr && "Render pass active during copy!");
        BONSAI_ASSERT(_activeComputePass == nullptr && "Compute pass active during copy!");
        BONSAI_ASSERT(src && dest && "Source and destination must be provided!");
        BONSAI_ASSERT(src->GetBackendAPI() == BackendAPI_WebGPU && dest->GetBackendAPI() == BackendAPI_WebGPU);

        WebGPUBuffer const* internalSrc = static_cast<WebGPUBuffer const*>(src);
        WebGPUBuffer const* internalDest = static_cast<WebGPUBuffer const*>(dest);
        if (size == RHI::WHOLE_SIZE) {
            size = wgpuBufferGetSize(internalSrc->GetBuffer());
        }

        wgpuCommandEncoderCopyBufferToBuffer(_commandEncoder, internalSrc->GetBuffer(), static_cast<uint64>(srcOffset), internalDest->GetBuffer(), static_cast<uint64>(destOffset), size);
    }

    void WebGPUCommandBuffer::CopyTextureToTexture(
        GPUTexture* src, GPUTexture* dest,
        TextureSubresource srcSubresource, Offset3D srcOffset,
        TextureSubresource destSubresource, Offset3D destOffset,
        Extent3D extent
    )
    {
        BONSAI_ASSERT(_activeRenderPass == nullptr && "Render pass active during copy!");
        BONSAI_ASSERT(_activeComputePass == nullptr && "Compute pass active during copy!");
        BONSAI_ASSERT(src && dest && "Source and destination must be provided!");
        BONSAI_ASSERT(src->GetBackendAPI() == BackendAPI_WebGPU && dest->GetBackendAPI() == BackendAPI_WebGPU);
        WebGPUTexture const* internalSrc = static_cast<WebGPUTexture const*>(src);
        WebGPUTexture const* internalDest = static_cast<WebGPUTexture const*>(dest);

        WGPUTexelCopyTextureInfo srcCopy{};
        srcCopy.texture = internalSrc->GetTexture();
        srcCopy.origin = WGPUOrigin3D{ static_cast<uint32>(srcOffset.x), static_cast<uint32>(srcOffset.y), static_cast<uint32>(srcOffset.z) };
        srcCopy.mipLevel = srcSubresource.mipLevel;
        srcCopy.aspect = GetTextureAspect(srcSubresource.aspectMask);

        WGPUTexelCopyTextureInfo destCopy{};
        destCopy.texture = internalDest->GetTexture();
        destCopy.origin = WGPUOrigin3D{ static_cast<uint32>(destOffset.x), static_cast<uint32>(destOffset.y), static_cast<uint32>(destOffset.z) };
        destCopy.mipLevel = destSubresource.mipLevel;
        destCopy.aspect = GetTextureAspect(destSubresource.aspectMask);

        WGPUExtent3D const copyExtent{ extent.width, extent.height, extent.depth };
        wgpuCommandEncoderCopyTextureToTexture(_commandEncoder, &srcCopy, &destCopy, &copyExtent);
    }

    void WebGPUCommandBuffer::CopyBufferToTexture(
        GPUBuffer* src, GPUTexture* dest,
        int64 bufferOffset, uint64 rowPitch, uint64 rowCount,
        TextureSubresource subresource, Offset3D textureOffset, Extent3D textureExtent
    )
    {
        BONSAI_ASSERT(_activeRenderPass == nullptr && "Render pass active during copy!");
        BONSAI_ASSERT(_activeComputePass == nullptr && "Compute pass active during copy!");
        BONSAI_ASSERT(src && dest && "Source and destination must be provided!");
        BONSAI_ASSERT(src->GetBackendAPI() == BackendAPI_WebGPU && dest->GetBackendAPI() == BackendAPI_WebGPU);
        WebGPUBuffer const* internalSrc = static_cast<WebGPUBuffer const*>(src);
        WebGPUTexture const* internalDest = static_cast<WebGPUTexture const*>(dest);

        WGPUTexelCopyBufferInfo bufferCopy{};
        bufferCopy.buffer = internalSrc->GetBuffer();
        bufferCopy.layout.offset = static_cast<uint64>(bufferOffset);
        bufferCopy.layout.bytesPerRow = rowPitch;
        bufferCopy.layout.rowsPerImage = rowCount;

        WGPUTexelCopyTextureInfo textureCopy{};
        textureCopy.texture = internalDest->GetTexture();
        textureCopy.origin = WGPUOrigin3D{ static_cast<uint32>(textureOffset.x), static_cast<uint32>(textureOffset.y), static_cast<uint32>(textureOffset.z) };
        textureCopy.mipLevel = subresource.mipLevel;
        textureCopy.aspect = GetTextureAspect(subresource.aspectMask);

        WGPUExtent3D const copyExtent{ textureExtent.width, textureExtent.height, textureExtent.depth };
        wgpuCommandEncoderCopyBufferToTexture(_commandEncoder, &bufferCopy, &textureCopy, &copyExtent);
    }

    void WebGPUCommandBuffer::CopyTextureToBuffer(
        GPUTexture* src, GPUBuffer* dest,
        int64 bufferOffset, uint64 rowPitch, uint64 rowCount,
        TextureSubresource subresource, Offset3D textureOffset, Extent3D textureExtent
    )
    {
        BONSAI_ASSERT(_activeRenderPass == nullptr && "Render pass active during copy!");
        BONSAI_ASSERT(_activeComputePass == nullptr && "Compute pass active during copy!");
        BONSAI_ASSERT(src && dest && "Source and destination must be provided!");
        BONSAI_ASSERT(src->GetBackendAPI() == BackendAPI_WebGPU && dest->GetBackendAPI() == BackendAPI_WebGPU);
        WebGPUTexture const* internalSrc = static_cast<WebGPUTexture const*>(src);
        WebGPUBuffer const* internalDest = static_cast<WebGPUBuffer const*>(dest);

        WGPUTexelCopyTextureInfo textureCopy{};
        textureCopy.texture = internalSrc->GetTexture();
        textureCopy.origin = WGPUOrigin3D{ static_cast<uint32>(textureOffset.x), static_cast<uint32>(textureOffset.y), static_cast<uint32>(textureOffset.z) };
        textureCopy.mipLevel = subresource.mipLevel;
        textureCopy.aspect = GetTextureAspect(subresource.aspectMask);

        WGPUTexelCopyBufferInfo bufferCopy{};
        bufferCopy.buffer = internalDest->GetBuffer();
        bufferCopy.layout.offset = static_cast<uint64>(bufferOffset);
        bufferCopy.layout.bytesPerRow = rowPitch;
        bufferCopy.layout.rowsPerImage = rowCount;

        WGPUExtent3D const copyExtent{ textureExtent.width, textureExtent.height, textureExtent.depth };
        wgpuCommandEncoderCopyTextureToBuffer(_commandEncoder, &textureCopy, &bufferCopy, &copyExtent);
    }

    void WebGPUCommandBuffer::SetPipeline(GPUPipeline* pipeline)
    {
        BONSAI_ASSERT(pipeline && "A pipeline must be provided!");
        BONSAI_ASSERT(pipeline->GetBackendAPI() == BackendAPI_WebGPU);
        WebGPUPipeline const* internalPipeline = static_cast<WebGPUPipeline const*>(pipeline);

        if (_activeRenderPass)
        {
            WGPURenderPipeline renderPipeline = internalPipeline->TryGetRenderPipeline();
            BONSAI_ASSERT(renderPipeline && "Provided pipeline is not a render pipeline, which was expected!");
            wgpuRenderPassEncoderSetPipeline(_activeRenderPass, renderPipeline);
        }
        
        if (_activeComputePass)
        {
            WGPUComputePipeline computePipeline = internalPipeline->TryGetComputePipeline();
            BONSAI_ASSERT(computePipeline && "Provided pipeline is not a compute pipeline, which was expected!");
            wgpuComputePassEncoderSetPipeline(_activeComputePass, computePipeline);
        }
    }

    void WebGPUCommandBuffer::SetDepthStencilState([[maybe_unused]] DepthStencilState* depthStencilState)
    {
        BONSAI_ASSERT(false && "Setting dynamic depth/stencil state is unsupported!");
    }

    void WebGPUCommandBuffer::SetBlendState([[maybe_unused]] BlendState* blendState)
    {
        BONSAI_ASSERT(false && "Setting dynamic blend state is unsupported!");
    }

    void WebGPUCommandBuffer::BindDescriptorSets(
        GPUPipelineLayout* layout,
        uint32 firstSet, uint32 setCount, GPUDescriptorSet** descriptorSets,
        uint32 dynamicOffsetCount, uint64* dynamicOffsets
    )
    {
        BONSAI_ASSERT(layout && "A pipeline layout must be provided!");
        BONSAI_ASSERT(layout->GetBackendAPI() == BackendAPI_WebGPU);
        BONSAI_ASSERT(dynamicOffsetCount <= RHI::MAX_DESCRIPTOR_BINDINGS && "Dynamic offset count must respect the maximum bound descriptor sets!");

        // Set up dynamic offsets how WebGPU expects them
        StaticArray<uint32, RHI::MAX_DESCRIPTOR_BINDINGS> internalDynamicOffsets;
        for (uint32 i = 0; i < dynamicOffsetCount; i++) {
            internalDynamicOffsets[i] = static_cast<uint32>(dynamicOffsets[i]);
        }

        // Set bind groups
        for (uint32 i = 0; i < setCount; i++)
        {
            GPUDescriptorSet const* descriptorSet = descriptorSets[i];
            BONSAI_ASSERT(descriptorSet->GetBackendAPI() == BackendAPI_WebGPU);

            uint32 const groupIndex = firstSet + i;
            WebGPUDescriptorSet const* internalDescriptorSet = static_cast<WebGPUDescriptorSet const*>(descriptorSet);

            if (_activeRenderPass) {
                wgpuRenderPassEncoderSetBindGroup(_activeRenderPass, groupIndex, internalDescriptorSet->GetBindGroup(), dynamicOffsetCount, internalDynamicOffsets.data());
            }

            if (_activeComputePass) {
                wgpuComputePassEncoderSetBindGroup(_activeComputePass, groupIndex, internalDescriptorSet->GetBindGroup(), dynamicOffsetCount, internalDynamicOffsets.data());
            }
        }
    }

    void WebGPUCommandBuffer::BeginRenderPass(RenderPassDesc* renderPassDesc)
    {
        BONSAI_ASSERT(renderPassDesc && "The render pass desc must not be null!");

        // Set up color attachments
        StaticArray<WGPURenderPassColorAttachment, RHI::MAX_RENDER_TARGETS> colorAttachments;
        for (uint32 i = 0; i < renderPassDesc->colorTargetCount; i++)
        {
            RenderPassTargetDesc const& target = renderPassDesc->colorTargets[i];
            BONSAI_ASSERT(target.texture && "The color target texture must not be null!");
            BONSAI_ASSERT(target.texture->GetBackendAPI() == BackendAPI_WebGPU);

            // Set up color attachment
            WebGPUTexture const* texture = static_cast<WebGPUTexture const*>(target.texture);
            WGPURenderPassColorAttachment& colorAttachment = colorAttachments[i];
            colorAttachment.nextInChain = nullptr;
            colorAttachment.view = texture->GetTextureView();
            colorAttachment.resolveTarget = nullptr;
            colorAttachment.loadOp = gLoadOpLUT[target.loadOp];
            colorAttachment.storeOp = gStoreOpLUT[target.storeOp];
            colorAttachment.clearValue = {
                target.clearValue.color.f32[0],
                target.clearValue.color.f32[1],
                target.clearValue.color.f32[2],
                target.clearValue.color.f32[3],
            };
            colorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

            // Handle optional resolve target
            if (target.resolveTarget)
            {
                BONSAI_ASSERT(target.resolveTarget->GetBackendAPI() == BackendAPI_WebGPU);
                WebGPUTexture const* resolveTarget = static_cast<WebGPUTexture const*>(target.resolveTarget);
                colorAttachment.resolveTarget = resolveTarget->GetTextureView();
            }
        }

        // Set up render pass desc
        WGPURenderPassDescriptor internalRenderPassDesc{};
        internalRenderPassDesc.nextInChain = nullptr;
        internalRenderPassDesc.label = {};
        internalRenderPassDesc.colorAttachmentCount = renderPassDesc->colorTargetCount;
        internalRenderPassDesc.colorAttachments = colorAttachments.data();
        internalRenderPassDesc.depthStencilAttachment = nullptr;
        internalRenderPassDesc.timestampWrites = nullptr;
        internalRenderPassDesc.occlusionQuerySet = nullptr;

        // Attach optional depth/stencil attachment
        WGPURenderPassDepthStencilAttachment depthStencilAttachment{};
        if (renderPassDesc->depthStencilTarget)
        {
            RenderPassTargetDesc const& target = *renderPassDesc->depthStencilTarget;
            BONSAI_ASSERT(target.texture && "The depth/stencil target texture must not be null!");
            BONSAI_ASSERT(target.texture->GetBackendAPI() == BackendAPI_WebGPU);

            // Set up depth/stencil attachment
            WebGPUTexture const* texture = static_cast<WebGPUTexture const*>(target.texture);
            depthStencilAttachment.view = texture->GetTextureView();
            depthStencilAttachment.depthReadOnly = false; // TODO(nemjit001): Handle read-only depth attachment
            depthStencilAttachment.depthLoadOp = gLoadOpLUT[target.loadOp];
            depthStencilAttachment.depthStoreOp = gStoreOpLUT[target.storeOp];
            depthStencilAttachment.depthClearValue = target.clearValue.depthStencil.depth;
            depthStencilAttachment.stencilReadOnly = false; // TODO(nemjit001): Handle read-only stencil attachment
            depthStencilAttachment.stencilLoadOp = gLoadOpLUT[target.loadOp];
            depthStencilAttachment.stencilStoreOp = gStoreOpLUT[target.storeOp];
            depthStencilAttachment.stencilClearValue = target.clearValue.depthStencil.stencil;

            internalRenderPassDesc.depthStencilAttachment = &depthStencilAttachment;
        }

        _activeRenderPass = wgpuCommandEncoderBeginRenderPass(_commandEncoder, &internalRenderPassDesc);
    }

    void WebGPUCommandBuffer::EndRenderPass()
    {
        BONSAI_ASSERT(_activeRenderPass && "No active render pass to end!");
        wgpuRenderPassEncoderEnd(_activeRenderPass);
        wgpuRenderPassEncoderRelease(_activeRenderPass);
        _activeRenderPass = nullptr;
    }

    void WebGPUCommandBuffer::BeginComputePass()
    {
        WGPUComputePassDescriptor internalComputePassDesc{};
        internalComputePassDesc.nextInChain = nullptr;
        internalComputePassDesc.label = {};
        internalComputePassDesc.timestampWrites = nullptr;

        _activeComputePass = wgpuCommandEncoderBeginComputePass(_commandEncoder, &internalComputePassDesc);
    }

    void WebGPUCommandBuffer::EndComputePass()
    {
        BONSAI_ASSERT(_activeComputePass && "No active compute pass to end!");
        wgpuComputePassEncoderEnd(_activeComputePass);
        wgpuComputePassEncoderRelease(_activeComputePass);
        _activeComputePass = nullptr;
    }

    void WebGPUCommandBuffer::Dispatch(uint32 sizeX, uint32 sizeY, uint32 sizeZ)
    {
        BONSAI_ASSERT(_activeComputePass && "A compute pass should be active when recording compute commands!");
        wgpuComputePassEncoderDispatchWorkgroups(_activeComputePass, sizeX, sizeY, sizeZ);
    }

    void WebGPUCommandBuffer::DispatchIndirect(GPUBuffer* dispatchCommands, int64 offset)
    {
        BONSAI_ASSERT(_activeComputePass && "A compute pass should be active when recording compute commands!");
        BONSAI_ASSERT(dispatchCommands && "Dispatch commands must be provided!");
        BONSAI_ASSERT(dispatchCommands->GetBackendAPI() == BackendAPI_WebGPU);
        WebGPUBuffer const* internalDispatchCommands = static_cast<WebGPUBuffer const*>(dispatchCommands);
        wgpuComputePassEncoderDispatchWorkgroupsIndirect(_activeComputePass, internalDispatchCommands->GetBuffer(), static_cast<uint64>(offset));
    }

    void WebGPUCommandBuffer::SetViewport(Viewport* viewport)
    {
        BONSAI_ASSERT(viewport && "The viewport must not be null!");
        BONSAI_ASSERT(_activeRenderPass && "A render pass should be active when recording draw commands!");
        wgpuRenderPassEncoderSetViewport(_activeRenderPass,
            viewport->x,
            viewport->y,
            viewport->width,
            viewport->height,
            viewport->minDepth,
            viewport->maxDepth
        );
    }

    void WebGPUCommandBuffer::SetScissor(Rect2D* scissor)
    {
        BONSAI_ASSERT(scissor && "The scissor rect must not be null!");
        BONSAI_ASSERT(_activeRenderPass && "A render pass should be active when recording draw commands!");
        wgpuRenderPassEncoderSetScissorRect(_activeRenderPass,
            static_cast<uint32>(scissor->offset.x),
            static_cast<uint32>(scissor->offset.y),
            scissor->extent.width,
            scissor->extent.height
        );
    }

    void WebGPUCommandBuffer::DrawIndexed(uint32 indexCount, uint32 instanceCount, uint32 firstIndex, int32 vertexOffset, uint32 firstInstance)
    {
        BONSAI_ASSERT(_activeRenderPass && "A render pass should be active when recording draw commands!");
        wgpuRenderPassEncoderDrawIndexed(_activeRenderPass, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }

    void WebGPUCommandBuffer::DrawIndexedIndirect(GPUBuffer* drawCommands, int64 offset)
    {
        BONSAI_ASSERT(_activeRenderPass && "A render pass should be active when recording draw commands!");
        BONSAI_ASSERT(drawCommands && "Draw commands must be provided!");
        BONSAI_ASSERT(drawCommands->GetBackendAPI() == BackendAPI_WebGPU);
        WebGPUBuffer const* internalDrawCommands = static_cast<WebGPUBuffer const*>(drawCommands);
        wgpuRenderPassEncoderDrawIndexedIndirect(_activeRenderPass, internalDrawCommands->GetBuffer(), static_cast<uint64>(offset));
    }

    void WebGPUCommandBuffer::Finish()
    {
        if (_commandEncoder)
        {
            WGPUCommandBufferDescriptor commandBufferDesc{};
            commandBufferDesc.nextInChain = nullptr;
            commandBufferDesc.label = {};

            _commandBuffer = wgpuCommandEncoderFinish(_commandEncoder, &commandBufferDesc);
            wgpuCommandEncoderRelease(_commandEncoder);
            _commandEncoder = nullptr;
        }
    }

    WebGPUGraphicsDevice::WebGPUGraphicsDevice(WGPUInstance instance, WGPUAdapter adapter, WGPUDevice device)
        :
        _instance(instance),
        _adapter(adapter),
        _device(device),
        _queue(wgpuDeviceGetQueue(device))
    {
        //
    }

    WebGPUGraphicsDevice::~WebGPUGraphicsDevice()
    {
        wgpuQueueRelease(_queue);
        wgpuDeviceRelease(_device);
        wgpuAdapterRelease(_adapter);
        wgpuInstanceRelease(_instance);
    }

    RHI::IGraphicsDevice* WebGPUGraphicsDevice::TryCreate([[maybe_unused]] RHI::GraphicsDeviceDesc const& graphicsDeviceDesc)
    {
        // Create instance
        WGPUInstanceDescriptor instanceDesc{};
        instanceDesc.nextInChain = nullptr;
        instanceDesc.features.nextInChain = nullptr;
        instanceDesc.features.timedWaitAnyEnable = false;
        instanceDesc.features.timedWaitAnyMaxCount = 0;

        WGPUInstance instance = wgpuCreateInstance(&instanceDesc);
        if (!instance) {
            return nullptr;
        }

        // Request adapter
        struct AdapterRequestData
        {
            bool complete = false;
            WGPUAdapter adapter = nullptr;
        } adapterRequestData;

        WGPURequestAdapterOptions adapterOpts{};
        adapterOpts.nextInChain = nullptr;
        adapterOpts.featureLevel = WGPUFeatureLevel_Core;
        adapterOpts.powerPreference = WGPUPowerPreference_HighPerformance;

        WGPURequestAdapterCallbackInfo requestAdapterCallbackInfo{};
        requestAdapterCallbackInfo.nextInChain = nullptr;
        requestAdapterCallbackInfo.mode = WGPUCallbackMode_AllowProcessEvents;
        requestAdapterCallbackInfo.userdata1 = &adapterRequestData;
        requestAdapterCallbackInfo.userdata2 = nullptr;
        requestAdapterCallbackInfo.callback = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, WGPUStringView message, void* ud1, void*) {
            AdapterRequestData& outRequestData = *static_cast<AdapterRequestData*>(ud1);
            outRequestData.complete = true;
            if (status == WGPURequestAdapterStatus_Success) {
                outRequestData.adapter = adapter;
            }
            else {
                PrintDebugString("[WebGPUGraphicsDevice]: %*.s", message.data, message.length);
            }
        };

        wgpuInstanceRequestAdapter(instance, &adapterOpts, requestAdapterCallbackInfo);
        while (!adapterRequestData.complete) {
            wgpuInstanceProcessEvents(instance);
        }

        if (!adapterRequestData.adapter)
        {
            wgpuInstanceRelease(instance);
            return nullptr;
        }

        // Request device
        struct DeviceRequestData
        {
            bool complete = false;
            WGPUDevice device = nullptr;
        } deviceRequestData;

        WGPUFeatureName requiredDeviceFeatures[] = {
            WGPUFeatureName_Depth32FloatStencil8,
            WGPUFeatureName_TimestampQuery,
            WGPUFeatureName_IndirectFirstInstance,
            WGPUFeatureName_Float32Filterable,
            WGPUFeatureName_Float32Blendable,
            WGPUFeatureName_DualSourceBlending,
        };

        WGPUDeviceDescriptor deviceDesc{};
        deviceDesc.nextInChain = nullptr;
        deviceDesc.label = WGPUStringView{ "WebGPU Device", WGPU_STRLEN };
        deviceDesc.requiredFeatureCount = std::size(requiredDeviceFeatures);
        deviceDesc.requiredFeatures = requiredDeviceFeatures;
        deviceDesc.requiredLimits = nullptr;
        deviceDesc.defaultQueue.nextInChain = nullptr;
        deviceDesc.defaultQueue.label = WGPUStringView{ "WebGPU Queue", WGPU_STRLEN };
        deviceDesc.deviceLostCallbackInfo = {};
        deviceDesc.uncapturedErrorCallbackInfo = {};

        WGPURequestDeviceCallbackInfo requestDeviceCallbackInfo{};
        requestDeviceCallbackInfo.nextInChain = nullptr;
        requestDeviceCallbackInfo.mode = WGPUCallbackMode_AllowProcessEvents;
        requestDeviceCallbackInfo.userdata1 = &deviceRequestData;
        requestDeviceCallbackInfo.userdata2 = nullptr;
        requestDeviceCallbackInfo.callback = [](WGPURequestDeviceStatus status, WGPUDevice device, WGPUStringView message, void* ud1, void*) {
            DeviceRequestData& outRequestData = *static_cast<DeviceRequestData*>(ud1);
            outRequestData.complete = true;
            if (status == WGPURequestDeviceStatus_Success) {
                outRequestData.device = device;
            }
            else {
                PrintDebugString("[WebGPUGraphicsDevice]: %*.s", message.data, message.length);
            }
        };

        wgpuAdapterRequestDevice(adapterRequestData.adapter, &deviceDesc, requestDeviceCallbackInfo);
        while (!deviceRequestData.complete) {
            wgpuInstanceProcessEvents(instance);
        }

        if (!deviceRequestData.device)
        {
            wgpuAdapterRelease(adapterRequestData.adapter);
            wgpuInstanceRelease(instance);
            return nullptr;
        }

        return new WebGPUGraphicsDevice(instance, adapterRequestData.adapter, deviceRequestData.device);
    }

    ShaderLang WebGPUGraphicsDevice::GetTargetShaderLanguage() const
    {
        return ShaderLang_WGSL;
    }

    GPUBuffer* WebGPUGraphicsDevice::CreateBuffer(GPUBufferDesc* bufferDesc)
    {
        if (!bufferDesc) {
            return nullptr;
        }

        // Set up buffer usage and supported map modes based on transfer flags and requested heap type
        WGPUBufferUsage usage = GetBufferUsage(bufferDesc->usage);
        if (bufferDesc->heapType == GPUHeapType_Default && !BitFlagIsSet(bufferDesc->usage, BufferUsage_TransferDst)) {
            usage |= WGPUBufferUsage_MapWrite; // host write only supported
        }
        else if (bufferDesc->heapType == GPUHeapType_Default && !BitFlagIsSet(bufferDesc->usage, BufferUsage_TransferSrc)) {
            usage |= WGPUBufferUsage_MapRead; // host read only supported
        }
        else if (bufferDesc->heapType == GPUHeapType_Readback && !BitFlagIsSet(bufferDesc->usage, BufferUsage_TransferSrc)) {
            usage |= WGPUBufferUsage_MapRead; // host read only supported
        }
    
        WGPUBufferDescriptor internalBufferDesc{};
        internalBufferDesc.nextInChain = nullptr;
        internalBufferDesc.label = {};
        internalBufferDesc.usage = usage;
        internalBufferDesc.size = bufferDesc->size;
        internalBufferDesc.mappedAtCreation = false;

        WGPUBuffer buffer = wgpuDeviceCreateBuffer(_device, &internalBufferDesc);
        if (!buffer) {
            return nullptr;
        }

        return new WebGPUBuffer(buffer);
    }

    void WebGPUGraphicsDevice::DestroyBuffer(GPUBuffer* buffer)
    {
        delete buffer;
    }

    GPUTexture* WebGPUGraphicsDevice::CreateTexture(GPUTextureDesc* textureDesc)
    {
        if (!textureDesc) {
            return nullptr;
        }

        WGPUTextureDescriptor internalTextureDesc{};
        internalTextureDesc.nextInChain = nullptr;
        internalTextureDesc.label = {};
        internalTextureDesc.usage = GetTextureUsage(textureDesc->usage);
        internalTextureDesc.dimension = gTextureDimensionLUT[textureDesc->textureType];
        internalTextureDesc.size = { textureDesc->size.width, textureDesc->size.height, std::max(textureDesc->size.depth, textureDesc->arrayLayers) }; // dpeth or array layers is interpreted as max value, kinda sneaky but should work OK
        internalTextureDesc.format = gTextureFormatLUT[textureDesc->format];
        internalTextureDesc.mipLevelCount = textureDesc->mipLevels;
        internalTextureDesc.sampleCount = textureDesc->sampleCount;
        internalTextureDesc.viewFormatCount = 0;
        internalTextureDesc.viewFormats = nullptr;

        WGPUTexture texture = wgpuDeviceCreateTexture(_device, &internalTextureDesc);
        if (!texture) {
            return nullptr;
        }

        WGPUTextureView view = wgpuTextureCreateView(texture, nullptr);
        if (!view)
        {
            wgpuTextureRelease(texture);
            return nullptr;
        }

        return new WebGPUTexture(texture, view);
    }

    GPUTexture* WebGPUGraphicsDevice::CreateTextureView(GPUTextureViewDesc* textureViewDesc)
    {
        if (!textureViewDesc) {
            return nullptr;
        }

        if (!textureViewDesc->texture || textureViewDesc->texture->GetBackendAPI() != BackendAPI_WebGPU) {
            return nullptr;
        }

        WebGPUTexture* texture = static_cast<WebGPUTexture*>(textureViewDesc->texture);
        WGPUTextureViewDescriptor internalTextureViewDesc{};
        internalTextureViewDesc.nextInChain = nullptr;
        internalTextureViewDesc.label = {};
        internalTextureViewDesc.format = gTextureFormatLUT[textureViewDesc->format];
        internalTextureViewDesc.dimension = gTextureViewDimensionLUT[textureViewDesc->viewType];
        internalTextureViewDesc.baseMipLevel = textureViewDesc->subresourceRange.baseMipLevel;
        internalTextureViewDesc.mipLevelCount = textureViewDesc->subresourceRange.levelCount;
        internalTextureViewDesc.baseArrayLayer = textureViewDesc->subresourceRange.baseArrayLayer;
        internalTextureViewDesc.arrayLayerCount = textureViewDesc->subresourceRange.layerCount;
        internalTextureViewDesc.aspect = GetTextureAspect(textureViewDesc->subresourceRange.aspectMask);
        internalTextureViewDesc.usage = wgpuTextureGetUsage(texture->GetTexture());

        WGPUTextureView view = wgpuTextureCreateView(texture->GetTexture(), &internalTextureViewDesc);
        if (!view) {
            return nullptr;
        }

        return new WebGPUTexture(nullptr, view);
    }

    void WebGPUGraphicsDevice::DestroyTexture(GPUTexture* texture)
    {
        delete texture;
    }

    GPUSampler* WebGPUGraphicsDevice::CreateSampler(GPUSamplerDesc* samplerDesc)
    {
        if (!samplerDesc) {
            return nullptr;
        }

        WGPUSamplerDescriptor internalSamplerDesc{};
        internalSamplerDesc.nextInChain = nullptr;
        internalSamplerDesc.label = {};
        internalSamplerDesc.addressModeU = gAddressModeLUT[samplerDesc->addressModeU];
        internalSamplerDesc.addressModeV = gAddressModeLUT[samplerDesc->addressModeV];
        internalSamplerDesc.addressModeW = gAddressModeLUT[samplerDesc->addressModeW];
        internalSamplerDesc.magFilter = gFilterModeLUT[samplerDesc->magFilter];
        internalSamplerDesc.minFilter = gFilterModeLUT[samplerDesc->minFilter];
        internalSamplerDesc.mipmapFilter = gMipmapFilterModeLUT[samplerDesc->mipmapMode];
        internalSamplerDesc.lodMinClamp = samplerDesc->minLOD;
        internalSamplerDesc.lodMaxClamp = samplerDesc->maxLOD;
        internalSamplerDesc.compare = samplerDesc->enableCompareOp ? gCompareFunctionLUT[samplerDesc->compareOp] : WGPUCompareFunction_Undefined;
        internalSamplerDesc.maxAnisotropy = std::clamp(samplerDesc->maxAnisotropy, 1u, 16u); // Force the anisotropy to be in range [1, 16]

        WGPUSampler sampler = wgpuDeviceCreateSampler(_device, &internalSamplerDesc);
        if (!sampler) {
            return nullptr;
        }

        return new WebGPUSampler(sampler);
    }

    void WebGPUGraphicsDevice::DestroySampler(GPUSampler* sampler)
    {
        delete sampler;
    }

    GPUDescriptorSetLayout* WebGPUGraphicsDevice::CreateDescriptorSetLayout(GPUDescriptorSetLayoutDesc* setLayoutDesc)
    {
        if (!setLayoutDesc) {
            return nullptr;
        }

        BONSAI_ASSERT(setLayoutDesc->bindingCount <= RHI::MAX_DESCRIPTOR_BINDINGS && "Descriptor set layout binding count exceed maximum allowed binding count!");
        StaticArray<WGPUBindGroupLayoutEntry, RHI::MAX_DESCRIPTOR_BINDINGS> bindGroupLayoutEntries;
        for (uint32 i = 0; i < setLayoutDesc->bindingCount; i++)
        {
            DescriptorSetLayoutBinding const& currentBinding = setLayoutDesc->bindings[i];

            WGPUBindGroupLayoutEntry& bindGroupLayoutEntry = bindGroupLayoutEntries[i];
            bindGroupLayoutEntry.nextInChain = nullptr;
            bindGroupLayoutEntry.binding = currentBinding.binding;
            bindGroupLayoutEntry.visibility = GetShaderStage(currentBinding.shaderStages);
            bindGroupLayoutEntry.buffer = {};
            bindGroupLayoutEntry.sampler = {};
            bindGroupLayoutEntry.texture = {};
            bindGroupLayoutEntry.storageTexture = {};

            // Set layout info based on descriptor type
            // FIXME(nemjit001): Handle unknown information somehow in this statement
            if (currentBinding.descriptorType == DescriptorType_Sampler)
            {
                bindGroupLayoutEntry.sampler.nextInChain = nullptr;
                bindGroupLayoutEntry.sampler.type = WGPUSamplerBindingType_Undefined;
            }
            else if (currentBinding.descriptorType == DescriptorType_CombinedImageSampler
                || currentBinding.descriptorType == DescriptorType_SampledImage
            )
            {
                bindGroupLayoutEntry.texture.nextInChain = nullptr;
                bindGroupLayoutEntry.texture.sampleType = WGPUTextureSampleType_Undefined;
                bindGroupLayoutEntry.texture.viewDimension = WGPUTextureViewDimension_Undefined;
                bindGroupLayoutEntry.texture.multisampled = false;
            }
            else if (currentBinding.descriptorType == DescriptorType_StorageImage)
            {
                bindGroupLayoutEntry.storageTexture.nextInChain = nullptr;
                bindGroupLayoutEntry.storageTexture.access = WGPUStorageTextureAccess_Undefined;
                bindGroupLayoutEntry.storageTexture.format = WGPUTextureFormat_Undefined;
                bindGroupLayoutEntry.storageTexture.viewDimension = WGPUTextureViewDimension_Undefined;
            }
            else if (currentBinding.descriptorType == DescriptorType_UniformBuffer
                || currentBinding.descriptorType == DescriptorType_StorageBuffer
                || currentBinding.descriptorType == DescriptorType_UniformBufferDynamic
                || currentBinding.descriptorCount == DescriptorType_StorageBufferDynamic
            )
            {
                bindGroupLayoutEntry.buffer.nextInChain = nullptr;
                bindGroupLayoutEntry.buffer.type = gBufferBindingTypeLUT[currentBinding.descriptorType];
                bindGroupLayoutEntry.buffer.hasDynamicOffset = false;
                bindGroupLayoutEntry.buffer.minBindingSize = 0; // Assume the whole buffer will be bound

                if (currentBinding.descriptorType == DescriptorType_UniformBufferDynamic
                    || currentBinding.descriptorType == DescriptorType_StorageBufferDynamic) {
                    bindGroupLayoutEntry.buffer.hasDynamicOffset = true;
                }
            }
        }

        WGPUBindGroupLayoutDescriptor internalBindGroupLayoutDesc{};
        internalBindGroupLayoutDesc.nextInChain = nullptr;
        internalBindGroupLayoutDesc.label = {};
        internalBindGroupLayoutDesc.entryCount = setLayoutDesc->bindingCount;
        internalBindGroupLayoutDesc.entries = bindGroupLayoutEntries.data();
        
        WGPUBindGroupLayout bindGroupLayout = wgpuDeviceCreateBindGroupLayout(_device, &internalBindGroupLayoutDesc);
        if (!bindGroupLayout) {
            return nullptr;
        }

        return new WebGPUDescriptorSetLayout(bindGroupLayout);
    }

    void WebGPUGraphicsDevice::DestroyDescriptorSetLayout(GPUDescriptorSetLayout* setLayout)
    {
        delete setLayout;
    }

    GPUDescriptorSet* WebGPUGraphicsDevice::CreateDescriptorSet(GPUDescriptorSetDesc* descriptorSetDesc)
    {
        if (!descriptorSetDesc) {
            return nullptr;
        }

        BONSAI_ASSERT(descriptorSetDesc->bindingCount <= RHI::MAX_DESCRIPTOR_BINDINGS && "Descriptor set binding count exceed maximum allowed binding count!");
        StaticArray<WGPUBindGroupEntry, RHI::MAX_DESCRIPTOR_BINDINGS> bindGroupEntries;
        for (uint32 i = 0; i < descriptorSetDesc->bindingCount; i++)
        {
            DescriptorSetBinding const& currentBinding = descriptorSetDesc->bindings[i];
            WGPUBindGroupEntry& bindGroupEntry = bindGroupEntries[i];
            bindGroupEntry.nextInChain = nullptr;
            bindGroupEntry.binding = currentBinding.binding;

            // Set buffer binding info
            if (currentBinding.bufferBindingInfo)
            {
                BONSAI_ASSERT(currentBinding.bufferBindingInfo->buffer && "Buffer in buffer binding must not be null!");
                BONSAI_ASSERT(currentBinding.bufferBindingInfo->buffer->GetBackendAPI() == BackendAPI_WebGPU);
                WebGPUBuffer const* buffer = static_cast<WebGPUBuffer const*>(currentBinding.bufferBindingInfo->buffer);
                bindGroupEntry.buffer = buffer->GetBuffer();
                bindGroupEntry.offset = currentBinding.bufferBindingInfo->offset;
                bindGroupEntry.size = currentBinding.bufferBindingInfo->range;
            }

            // Set texture binding info
            if (currentBinding.textureBindingInfo)
            {
                if (currentBinding.textureBindingInfo->texture)
                {
                    BONSAI_ASSERT(currentBinding.textureBindingInfo->texture->GetBackendAPI() == BackendAPI_WebGPU);
                    WebGPUTexture const* texture = static_cast<WebGPUTexture const*>(currentBinding.textureBindingInfo->texture);
                    bindGroupEntry.textureView = texture->GetTextureView();
                }

                if (currentBinding.textureBindingInfo->sampler)
                {
                    BONSAI_ASSERT(currentBinding.textureBindingInfo->sampler->GetBackendAPI() == BackendAPI_WebGPU);
                    WebGPUSampler const* sampler = static_cast<WebGPUSampler const*>(currentBinding.textureBindingInfo->sampler);
                    bindGroupEntry.sampler = sampler->GetSampler();                    
                }
            }
        }

        BONSAI_ASSERT(descriptorSetDesc->setLayout && "Descriptor set layout must not be null!");
        BONSAI_ASSERT(descriptorSetDesc->setLayout->GetBackendAPI() == BackendAPI_WebGPU);
        WebGPUDescriptorSetLayout const* descriptorSetLayout = static_cast<WebGPUDescriptorSetLayout const*>(descriptorSetDesc->setLayout);

        WGPUBindGroupDescriptor internalBindGroupDesc{};
        internalBindGroupDesc.nextInChain = nullptr;
        internalBindGroupDesc.label = {};
        internalBindGroupDesc.layout = descriptorSetLayout->GetBindGroupLayout();
        internalBindGroupDesc.entryCount = descriptorSetDesc->bindingCount;
        internalBindGroupDesc.entries = bindGroupEntries.data();

        WGPUBindGroup bindGroup = wgpuDeviceCreateBindGroup(_device, &internalBindGroupDesc);
        if (!bindGroup) {
            return nullptr;
        }

        return new WebGPUDescriptorSet(bindGroup);
    }

    void WebGPUGraphicsDevice::DestroyDescriptorSet(GPUDescriptorSet* descriptorSet)
    {
        delete descriptorSet;
    }

    GPUPipelineLayout* WebGPUGraphicsDevice::CreatePipelineLayout(GPUPipelineLayoutDesc* pipelineLayoutDesc)
    {
        if (!pipelineLayoutDesc) {
            return nullptr;
        }

        BONSAI_ASSERT(pipelineLayoutDesc->setLayoutCount <= RHI::MAX_DESCRIPTOR_SETS
            && "Pipeline layout set layout count exceed maximum allowed set layout count!");
        StaticArray<WGPUBindGroupLayout, RHI::MAX_DESCRIPTOR_SETS> bindGroupLayouts;
        for (uint32 i = 0; i < pipelineLayoutDesc->setLayoutCount; i++)
        {
            BONSAI_ASSERT(pipelineLayoutDesc->setLayouts[i] != nullptr);
            BONSAI_ASSERT(pipelineLayoutDesc->setLayouts[i]->GetBackendAPI() == BackendAPI_WebGPU);
            WebGPUDescriptorSetLayout const* descriptorSetLayout = static_cast<WebGPUDescriptorSetLayout const*>(pipelineLayoutDesc->setLayouts[i]);
            bindGroupLayouts[i] = descriptorSetLayout->GetBindGroupLayout();
        }

        WGPUPipelineLayoutDescriptor internalPipelineLayoutDesc{};
        internalPipelineLayoutDesc.nextInChain = nullptr;
        internalPipelineLayoutDesc.label = {};
        internalPipelineLayoutDesc.bindGroupLayoutCount = pipelineLayoutDesc->setLayoutCount;
        internalPipelineLayoutDesc.bindGroupLayouts = bindGroupLayouts.data();

        WGPUPipelineLayout pipelineLayout = wgpuDeviceCreatePipelineLayout(_device, &internalPipelineLayoutDesc);
        if (!pipelineLayout) {
            return nullptr;
        }

        return new WebGPUPipelineLayout(pipelineLayout);
    }

    void WebGPUGraphicsDevice::DestroyPipelineLayout(GPUPipelineLayout* pipelineLayout)
    {
        delete pipelineLayout;
    }

    GPUPipeline* WebGPUGraphicsDevice::CreateGraphicsPipeline(GPUGraphicsPipelineDesc* pipelineDesc)
    {
        if (!pipelineDesc) {
            return nullptr;
        }

        BONSAI_ASSERT(pipelineDesc->pipelineLayout != nullptr && "Pipeline layout may not be null!");
        BONSAI_ASSERT(pipelineDesc->pipelineLayout->GetBackendAPI() == BackendAPI_WebGPU);
        WebGPUPipelineLayout const* pipelineLayout = static_cast<WebGPUPipelineLayout const*>(pipelineDesc->pipelineLayout);

        WGPURenderPipelineDescriptor renderPipelineDesc{};
        renderPipelineDesc.nextInChain = nullptr;
        renderPipelineDesc.label = {};
        renderPipelineDesc.layout = pipelineLayout->GetPipelineLayout();
        renderPipelineDesc.vertex = {}; // Initialization is only done if a vertex shader is present.
        renderPipelineDesc.fragment = nullptr; // Initialization is only done if a pixel shader is present.
        renderPipelineDesc.primitive.nextInChain = nullptr;
        renderPipelineDesc.primitive.topology = gPrimitiveTopologyLUT[pipelineDesc->rasterState.primitiveTopology];
        renderPipelineDesc.primitive.stripIndexFormat = WGPUIndexFormat_Undefined; // TODO(nemjit001): add support for strip index formats
        renderPipelineDesc.primitive.frontFace = pipelineDesc->rasterState.frontFaceCounterClockwise ? WGPUFrontFace_CCW : WGPUFrontFace_CW;
        renderPipelineDesc.primitive.cullMode = gCullModeLUT[pipelineDesc->rasterState.cullMode];
        renderPipelineDesc.primitive.unclippedDepth = false; // TODO(nemjit001): add support for unclipped depth
        renderPipelineDesc.multisample.nextInChain = nullptr;
        renderPipelineDesc.multisample.count = pipelineDesc->rasterState.sampleCount;
        renderPipelineDesc.multisample.alphaToCoverageEnabled = pipelineDesc->rasterState.alphaToCoverageEnable;
        renderPipelineDesc.multisample.mask = UINT32_MAX; // TODO(nemjit001): add support for msaa sample mask
        renderPipelineDesc.depthStencil = nullptr;

        // Set up color target state
        StaticArray<WGPUColorTargetState, RHI::MAX_RENDER_TARGETS> colorTargets;
        for (uint32 i = 0; i < pipelineDesc->renderTargetCount; i++)
        {
            WGPUColorTargetState& target = colorTargets[i];
            target.nextInChain = nullptr;
            target.format = gTextureFormatLUT[pipelineDesc->renderTargetFormats[i]];
            target.blend = nullptr;
            target.writeMask = WGPUColorWriteMask_All; // Assume all unless specified differently in blend state.
        }

        // Set up depth/stencil state
        WGPUDepthStencilState depthStencilState{};
        if (pipelineDesc->depthStencilState)
        {
            DepthStencilState const& pipelineDepthStencilState = *pipelineDesc->depthStencilState;

            depthStencilState.nextInChain = nullptr;
            depthStencilState.format = gTextureFormatLUT[pipelineDesc->depthStencilFormat];
            depthStencilState.depthWriteEnabled = pipelineDepthStencilState.depthWrite ? WGPUOptionalBool_True : WGPUOptionalBool_False;
            depthStencilState.depthCompare = pipelineDepthStencilState.depthTest ? gCompareFunctionLUT[pipelineDepthStencilState.depthCompareOp] : WGPUCompareFunction_Undefined;
            depthStencilState.stencilFront = { WGPUCompareFunction_Always, WGPUStencilOperation_Keep, WGPUStencilOperation_Keep, WGPUStencilOperation_Keep }; // TODO(nemjit001): add support for stencil ops
            depthStencilState.stencilBack = { WGPUCompareFunction_Always, WGPUStencilOperation_Keep, WGPUStencilOperation_Keep, WGPUStencilOperation_Keep };
            depthStencilState.stencilReadMask = UINT32_MAX;
            depthStencilState.stencilWriteMask = UINT32_MAX;
            depthStencilState.depthBias = static_cast<uint32>(pipelineDepthStencilState.depthBias);
            depthStencilState.depthBiasSlopeScale = pipelineDepthStencilState.depthBiasSlopeFactor;
            depthStencilState.depthBiasClamp = pipelineDepthStencilState.depthBiasClamp;
            
            renderPipelineDesc.depthStencil = &depthStencilState;
        }

        // Set up color target blend state
        StaticArray<WGPUBlendState, RHI::MAX_RENDER_TARGETS> targetBlendStates;
        if (pipelineDesc->blendState)
        {
            BlendState const& pipelineBlendState = *pipelineDesc->blendState;
            for (uint32 i = 0; i < pipelineDesc->renderTargetCount; i++)
            {
                RenderTargetBlendState const& currentBlendState = pipelineBlendState.blendTargets[i];

                WGPUBlendState& blendState = targetBlendStates[i];
                blendState.color.srcFactor = gBlendFactorLUT[currentBlendState.srcColorBlendFactor];
                blendState.color.dstFactor = gBlendFactorLUT[currentBlendState.destColorBlendFactor];
                blendState.color.operation = gBlendOperationLUT[currentBlendState.colorBlendOp];
                blendState.alpha.srcFactor = gBlendFactorLUT[currentBlendState.srcAlphaBlendFactor];
                blendState.alpha.dstFactor = gBlendFactorLUT[currentBlendState.destAlphaBlendFactor];
                blendState.alpha.operation = gBlendOperationLUT[currentBlendState.alphaBlendOp];

                WGPUColorTargetState& target = colorTargets[i];
                target.blend = currentBlendState.blendEnable ? &blendState : nullptr;
                target.writeMask = GetColorWriteMask(currentBlendState.colorWriteMask);
            }
        }

        // Set up vertex state
        WGPUShaderModule vertexShaderModule = nullptr;
        if (pipelineDesc->vertexShader)
        {
            WGPUShaderSourceWGSL shaderSource{};
            shaderSource.chain.sType = WGPUSType_ShaderSourceWGSL;
            shaderSource.chain.next = nullptr;
            shaderSource.code.data = static_cast<char const*>(pipelineDesc->vertexShader->code);
            shaderSource.code.length = pipelineDesc->vertexShader->size;

            WGPUShaderModuleDescriptor shaderModuleDesc{};
            shaderModuleDesc.nextInChain = &shaderSource.chain;
            shaderModuleDesc.label = {};
            
            vertexShaderModule = wgpuDeviceCreateShaderModule(_device, &shaderModuleDesc);

            renderPipelineDesc.vertex.nextInChain = nullptr;
            renderPipelineDesc.vertex.module = vertexShaderModule;
            renderPipelineDesc.vertex.entryPoint = WGPUStringView{ pipelineDesc->vertexShaderEntrypoint, WGPU_STRLEN };
            renderPipelineDesc.vertex.constantCount = 0;
            renderPipelineDesc.vertex.constants = nullptr;
            renderPipelineDesc.vertex.bufferCount = 0; // TODO(nemjit001): Add vertex input layout support.
            renderPipelineDesc.vertex.buffers = nullptr;
        }
        
        // Set up fragment state
        WGPUShaderModule fragmentShaderModule = nullptr;
        WGPUFragmentState fragmentState{};
        if (pipelineDesc->pixelShader)
        {
            WGPUShaderSourceWGSL shaderSource{};
            shaderSource.chain.sType = WGPUSType_ShaderSourceWGSL;
            shaderSource.chain.next = nullptr;
            shaderSource.code.data = static_cast<char const*>(pipelineDesc->pixelShader->code);
            shaderSource.code.length = pipelineDesc->pixelShader->size;

            WGPUShaderModuleDescriptor shaderModuleDesc{};
            shaderModuleDesc.nextInChain = &shaderSource.chain;
            shaderModuleDesc.label = {};

            fragmentShaderModule = wgpuDeviceCreateShaderModule(_device, &shaderModuleDesc);

            fragmentState.nextInChain = nullptr;
            fragmentState.module = fragmentShaderModule;
            fragmentState.entryPoint = WGPUStringView{ pipelineDesc->pixelShaderEntrypoint, WGPU_STRLEN };
            fragmentState.constantCount = 0;
            fragmentState.constants = nullptr;
            fragmentState.targetCount = pipelineDesc->renderTargetCount;
            fragmentState.targets = colorTargets.data();

            renderPipelineDesc.fragment = &fragmentState;
        }

        WGPURenderPipeline pipeline = wgpuDeviceCreateRenderPipeline(_device, &renderPipelineDesc);
        if (vertexShaderModule) {
            wgpuShaderModuleRelease(vertexShaderModule);
        }

        if (fragmentShaderModule) {
            wgpuShaderModuleRelease(fragmentShaderModule);
        }

        if (!pipeline) {
            return nullptr;
        }

        return new WebGPUPipeline(pipeline);
    }

    GPUPipeline* WebGPUGraphicsDevice::CreateComputePipeline(GPUComputePipelineDesc* pipelineDesc)
    {
        if (!pipelineDesc) {
            return nullptr;
        }

        BONSAI_ASSERT(pipelineDesc->pipelineLayout != nullptr && "Pipeline layout may not be null!");
        BONSAI_ASSERT(pipelineDesc->pipelineLayout->GetBackendAPI() == BackendAPI_WebGPU);
        WebGPUPipelineLayout const* pipelineLayout = static_cast<WebGPUPipelineLayout const*>(pipelineDesc->pipelineLayout);

        WGPUShaderSourceWGSL computeShaderSource{};
        computeShaderSource.chain.sType = WGPUSType_ShaderSourceWGSL;
        computeShaderSource.chain.next = nullptr;
        computeShaderSource.code.data = static_cast<char const*>(pipelineDesc->computeShader.code);
        computeShaderSource.code.length = pipelineDesc->computeShader.size;

        WGPUShaderModuleDescriptor computeShaderDesc{};
        computeShaderDesc.nextInChain = &computeShaderSource.chain;
        computeShaderDesc.label = {};

        WGPUShaderModule computeShader = wgpuDeviceCreateShaderModule(_device, &computeShaderDesc);
        if (!computeShader) {
            return nullptr;
        }

        WGPUComputePipelineDescriptor computePipelineDesc{};
        computePipelineDesc.nextInChain = nullptr;
        computePipelineDesc.label = {};
        computePipelineDesc.layout = pipelineLayout->GetPipelineLayout();
        computePipelineDesc.compute.nextInChain = nullptr;
        computePipelineDesc.compute.module = computeShader;
        computePipelineDesc.compute.entryPoint = WGPUStringView{ pipelineDesc->entrypoint, WGPU_STRLEN };
        computePipelineDesc.compute.constantCount = 0;
        computePipelineDesc.compute.constants = nullptr;

        WGPUComputePipeline pipeline = wgpuDeviceCreateComputePipeline(_device, &computePipelineDesc);
        wgpuShaderModuleRelease(computeShader);
        
        if (!pipeline) {
            return nullptr;
        }

        return new WebGPUPipeline(pipeline);
    }

    void WebGPUGraphicsDevice::DestroyPipeline(GPUPipeline* pipeline)
    {
        delete pipeline;
    }

    IGPUCommandBuffer* WebGPUGraphicsDevice::CreateCommandBuffer([[maybe_unused]] GPUQueueType queue)
    {
        WGPUCommandEncoderDescriptor commandEncoderDesc{};
        commandEncoderDesc.nextInChain = nullptr;
        commandEncoderDesc.label = {};

        WGPUCommandEncoder commandEncoder = wgpuDeviceCreateCommandEncoder(_device, &commandEncoderDesc);
        if (!commandEncoder) {
            return nullptr;
        }

        return new WebGPUCommandBuffer(commandEncoder);
    }

    void WebGPUGraphicsDevice::DestroyCommandBuffer(IGPUCommandBuffer* commandBuffer)
    {
        delete commandBuffer;
    }

    GraphicsDeviceResult WebGPUGraphicsDevice::Submit([[maybe_unused]] GPUQueueType queue, IGPUCommandBuffer** commandBuffers, uint32 commandBufferCount)
    {
        BONSAI_ASSERT(commandBufferCount <= RHI::MAX_COMMAND_BUFFER_SUBMISSIONS && "Command buffer count exceeds maximum allowed submission count!");
        StaticArray<WGPUCommandBuffer, RHI::MAX_COMMAND_BUFFER_SUBMISSIONS> submissions;
        for (uint32 i = 0; i < commandBufferCount; i++)
        {
            if (commandBuffers[i]->GetBackendAPI() != BackendAPI_WebGPU) {
                return GraphicsDeviceResult_BadSubmit;
            }

            WebGPUCommandBuffer const* commandBuffer = static_cast<WebGPUCommandBuffer const*>(commandBuffers[i]);
            submissions[i] = commandBuffer->GetCommandBuffer();
            if (submissions[i] == nullptr) {
                return GraphicsDeviceResult_BadSubmit;
            }
        }

        wgpuQueueSubmit(_queue, commandBufferCount, submissions.data());
        return GraphicsDeviceResult_Ok;
    }
    
    GraphicsDeviceResult WebGPUGraphicsDevice::MapBuffer(GPUBuffer* buffer, int64 offset, uint64 size, void** outData) const
    {
        BONSAI_ASSERT(buffer && "A buffer must be provided!");
        BONSAI_ASSERT(outData != nullptr && "An output pointer value for the mapped buffer region must be provided!");
        BONSAI_ASSERT(buffer->GetBackendAPI() == BackendAPI_WebGPU);
        WebGPUBuffer const* internalBuffer = static_cast<WebGPUBuffer const*>(buffer);
        *outData = nullptr;

        // Determine map size
        if (size == RHI::WHOLE_SIZE) {
            size = wgpuBufferGetSize(internalBuffer->GetBuffer());
        }

        // Determine map mode based on usage flags
        WGPUMapMode bufferMapMode = WGPUMapMode_None;
        WGPUBufferUsage const usage = wgpuBufferGetUsage(internalBuffer->GetBuffer());
        if (BitFlagIsSet(usage, WGPUBufferUsage_MapRead)) {
            bufferMapMode = WGPUMapMode_Read;
        }
        else if (BitFlagIsSet(usage, WGPUBufferUsage_MapWrite)) {
            bufferMapMode = WGPUMapMode_Write;
        }

        // Set up map callback
        bool mapComplete = false;
        WGPUBufferMapCallbackInfo mapCallbackInfo{};
        mapCallbackInfo.nextInChain = nullptr;
        mapCallbackInfo.mode = WGPUCallbackMode_AllowProcessEvents;
        mapCallbackInfo.userdata1 = &mapComplete;
        mapCallbackInfo.userdata2 = nullptr;
        mapCallbackInfo.callback = [](WGPUMapAsyncStatus status, WGPUStringView message, void* ud1, void*) {
            bool& outMapComplete = *static_cast<bool*>(ud1);
            if (status == WGPUMapAsyncStatus_Success) {
                outMapComplete = true;
            }
            else {
                PrintDebugString("[WebGPUGraphicsDevice]: %*.s", message.data, message.length);
            }
        };

        // Actually map buffer
        wgpuBufferMapAsync(internalBuffer->GetBuffer(), bufferMapMode, offset, size, mapCallbackInfo);
        while (!mapComplete) {
            wgpuInstanceProcessEvents(_instance);
        }

        *outData = wgpuBufferGetMappedRange(internalBuffer->GetBuffer(), offset, size);
        return GraphicsDeviceResult_Ok;
    }

    void WebGPUGraphicsDevice::UnmapBuffer(GPUBuffer* buffer) const
    {
        BONSAI_ASSERT(buffer && "A buffer must be provided!");
        BONSAI_ASSERT(buffer->GetBackendAPI() == BackendAPI_WebGPU);
        WebGPUBuffer const* internalBuffer = static_cast<WebGPUBuffer const*>(buffer);
        wgpuBufferUnmap(internalBuffer->GetBuffer());
    }

    void WebGPUGraphicsDevice::WaitIdle()
    {
        // TODO(nemjit001): Implement callback-based wait on submits (timeline semaphore?)
    }
} // namespace RHI

#endif //BONSAI_RHI_WEBGPU_SUPPORT
