#include "WebGPUGraphicsDevice.hpp"
#if BONSAI_RHI_WEBGPU_SUPPORT

#include <Containers/DynamicArray.hpp>
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
    static constexpr WGPUTextureViewDimension gTextureViewDimensionsLUT[] = {
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
        BONSAI_ASSERT(src->GetBackendAPI() == BackendAPI_WebGPU && dest->GetBackendAPI() == BackendAPI_WebGPU);
        WebGPUBuffer const* internalSrc = static_cast<WebGPUBuffer const*>(src);
        WebGPUBuffer const* internalDest = static_cast<WebGPUBuffer const*>(dest);
        wgpuCommandEncoderCopyBufferToBuffer(_commandEncoder, internalSrc->GetBuffer(), static_cast<uint64>(srcOffset), internalDest->GetBuffer(), static_cast<uint64>(destOffset), size);
    }

    void WebGPUCommandBuffer::CopyTextureToTexture(
        GPUTexture* src, GPUTexture* dest,
        TextureSubresource srcSubresource, Offset3D srcOffset,
        TextureSubresource destSubresource, Offset3D destOffset,
        Extent3D extent
    )
    {
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

    void WebGPUCommandBuffer::SetDepthStencilState(DepthStencilState* depthStencilState)
    {
        //
    }

    void WebGPUCommandBuffer::SetBlendState(BlendState* blendState)
    {
        //
    }

    void WebGPUCommandBuffer::BeginRenderPass(RenderPassDesc* renderPassDesc)
    {
        BONSAI_ASSERT(renderPassDesc && "The render pass desc must not be null!");

        // Set up color attachments
        DynamicArray<WGPURenderPassColorAttachment> colorAttachments(renderPassDesc->colorTargetCount);
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
                target.clearValue.color.float32[0],
                target.clearValue.color.float32[1],
                target.clearValue.color.float32[2],
                target.clearValue.color.float32[3],
            };
            colorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

            // Handle optional resolve target
            if (target.resolveTarget)
            {
                BONSAI_ASSERT(target.resolveTarget->GetBackendAPI() == BackendAPI_WebGPU);
                WebGPUTexture const* resolveTarget = static_cast<WebGPUTexture const*>(target.resolveTarget);
                colorAttachment.resolveTarget = resolveTarget->GetTextureView();
            }

            colorAttachments.push_back(colorAttachment);
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

    void WebGPUCommandBuffer::Dispatch(uint32 sizeX, uint32 sizeY, uint32 sizeZ)
    {
        BONSAI_ASSERT(_activeComputePass && "A compute pass should be active when recording compute commands!");
        wgpuComputePassEncoderDispatchWorkgroups(_activeComputePass, sizeX, sizeY, sizeZ);
    }

    void WebGPUCommandBuffer::DispatchIndirect(GPUBuffer* dispatchCommands, int64 offset)
    {
        BONSAI_ASSERT(_activeComputePass && "A compute pass should be active when recording compute commands!");
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
        WGPURequestAdapterOptions adapterOpts{};
        adapterOpts.nextInChain = nullptr;
        adapterOpts.featureLevel = WGPUFeatureLevel_Core;
        adapterOpts.powerPreference = WGPUPowerPreference_HighPerformance;

        WGPUAdapter adapter = nullptr;
        WGPURequestAdapterCallbackInfo requestAdapterCallbackInfo{};
        requestAdapterCallbackInfo.nextInChain = nullptr;
        requestAdapterCallbackInfo.mode = WGPUCallbackMode_AllowProcessEvents;
        requestAdapterCallbackInfo.userdata1 = &adapter;
        requestAdapterCallbackInfo.userdata2 = nullptr;
        requestAdapterCallbackInfo.callback = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, WGPUStringView message, void* ud1, void*) {
            WGPUAdapter& outAdapter = *static_cast<WGPUAdapter*>(ud1);
            outAdapter = nullptr;
            if (status == WGPURequestAdapterStatus_Success) {
                outAdapter = adapter;
            }
            else {
                PrintDebugString("WebGPU error: %*.s", message.data, message.length);
            }
        };

        wgpuInstanceRequestAdapter(instance, &adapterOpts, requestAdapterCallbackInfo);
        wgpuInstanceProcessEvents(instance);
        if (!adapter) {
            return nullptr;
        }

        // Request device
        WGPUDeviceDescriptor deviceDesc{};
        deviceDesc.nextInChain = nullptr;
        deviceDesc.label = WGPUStringView{ "WebGPU Device", WGPU_STRLEN };
        deviceDesc.requiredFeatureCount = 0;
        deviceDesc.requiredFeatures = nullptr;
        deviceDesc.requiredLimits = nullptr;
        deviceDesc.defaultQueue.nextInChain = nullptr;
        deviceDesc.defaultQueue.label = WGPUStringView{ "WebGPU Queue", WGPU_STRLEN };
        deviceDesc.deviceLostCallbackInfo = {};
        deviceDesc.uncapturedErrorCallbackInfo = {};

        WGPUDevice device = nullptr;
        WGPURequestDeviceCallbackInfo requestDeviceCallbackInfo{};
        requestDeviceCallbackInfo.nextInChain = nullptr;
        requestDeviceCallbackInfo.mode = WGPUCallbackMode_AllowProcessEvents;
        requestDeviceCallbackInfo.userdata1 = &device;
        requestDeviceCallbackInfo.userdata2 = nullptr;
        requestDeviceCallbackInfo.callback = [](WGPURequestDeviceStatus status, WGPUDevice device, WGPUStringView message, void* ud1, void*) {
            WGPUDevice& outDevice = *static_cast<WGPUDevice*>(ud1);
            outDevice = nullptr;
            if (status == WGPURequestDeviceStatus_Success) {
                outDevice = device;
            }
            else {
                PrintDebugString("WebGPU error: %*.s", message.data, message.length);
            }
        };

        wgpuAdapterRequestDevice(adapter, &deviceDesc, requestDeviceCallbackInfo);
        wgpuInstanceProcessEvents(instance);
        if (!device) {
            return nullptr;
        }

        return new WebGPUGraphicsDevice(instance, adapter, device);
    }

    GPUBuffer* WebGPUGraphicsDevice::CreateBuffer(GPUBufferDesc* bufferDesc)
    {
        if (!bufferDesc) {
            return nullptr;
        }

        WGPUBufferDescriptor wgpuBufferDesc{};
        wgpuBufferDesc.nextInChain = nullptr;
        wgpuBufferDesc.label = {};
        wgpuBufferDesc.usage = GetBufferUsage(bufferDesc->usage);
        wgpuBufferDesc.size = bufferDesc->size;
        wgpuBufferDesc.mappedAtCreation = false;

        WGPUBuffer buffer = wgpuDeviceCreateBuffer(_device, &wgpuBufferDesc);
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

        WGPUTextureDescriptor wgpuTextureDesc{};
        wgpuTextureDesc.nextInChain = nullptr;
        wgpuTextureDesc.label = {};
        wgpuTextureDesc.usage = GetTextureUsage(textureDesc->usage);
        wgpuTextureDesc.dimension = gTextureDimensionLUT[textureDesc->textureType];
        wgpuTextureDesc.size = { textureDesc->size.width, textureDesc->size.height, std::max(textureDesc->size.depth, textureDesc->arrayLayers) }; // dpeth or array layers is interpreted as max value, kinda sneaky but should work OK
        wgpuTextureDesc.format = gTextureFormatLUT[textureDesc->format];
        wgpuTextureDesc.mipLevelCount = textureDesc->mipLevels;
        wgpuTextureDesc.sampleCount = textureDesc->sampleCount;
        wgpuTextureDesc.viewFormatCount = 0;
        wgpuTextureDesc.viewFormats = nullptr;

        WGPUTexture texture = wgpuDeviceCreateTexture(_device, &wgpuTextureDesc);
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
        WGPUTextureViewDescriptor wgpuTextureViewDesc{};
        wgpuTextureViewDesc.nextInChain = nullptr;
        wgpuTextureViewDesc.label = {};
        wgpuTextureViewDesc.format = gTextureFormatLUT[textureViewDesc->format];
        wgpuTextureViewDesc.dimension = gTextureViewDimensionsLUT[textureViewDesc->viewType];
        wgpuTextureViewDesc.baseMipLevel = textureViewDesc->subresourceRange.baseMipLevel;
        wgpuTextureViewDesc.mipLevelCount = textureViewDesc->subresourceRange.levelCount;
        wgpuTextureViewDesc.baseArrayLayer = textureViewDesc->subresourceRange.baseArrayLayer;
        wgpuTextureViewDesc.arrayLayerCount = textureViewDesc->subresourceRange.layerCount;
        wgpuTextureViewDesc.aspect = GetTextureAspect(textureViewDesc->subresourceRange.aspectMask);
        wgpuTextureViewDesc.usage = wgpuTextureGetUsage(texture->GetTexture());

        WGPUTextureView view = wgpuTextureCreateView(texture->GetTexture(), &wgpuTextureViewDesc);
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

        WGPUSamplerDescriptor wgpuSamplerDesc{};
        wgpuSamplerDesc.nextInChain = nullptr;
        wgpuSamplerDesc.label = {};
        wgpuSamplerDesc.addressModeU = gAddressModeLUT[samplerDesc->addressModeU];
        wgpuSamplerDesc.addressModeV = gAddressModeLUT[samplerDesc->addressModeV];
        wgpuSamplerDesc.addressModeW = gAddressModeLUT[samplerDesc->addressModeW];
        wgpuSamplerDesc.magFilter = gFilterModeLUT[samplerDesc->magFilter];
        wgpuSamplerDesc.minFilter = gFilterModeLUT[samplerDesc->minFilter];
        wgpuSamplerDesc.mipmapFilter = gMipmapFilterModeLUT[samplerDesc->mipmapMode];
        wgpuSamplerDesc.lodMinClamp = samplerDesc->minLOD;
        wgpuSamplerDesc.lodMaxClamp = samplerDesc->maxLOD;
        wgpuSamplerDesc.compare = samplerDesc->enableCompareOp ? gCompareFunctionLUT[samplerDesc->compareOp] : WGPUCompareFunction_Undefined;
        wgpuSamplerDesc.maxAnisotropy = std::clamp(samplerDesc->maxAnisotropy, 1u, 16u); // Force the anisotropy to be in range [1, 16]

        WGPUSampler sampler = wgpuDeviceCreateSampler(_device, &wgpuSamplerDesc);
        if (!sampler) {
            return nullptr;
        }

        return new WebGPUSampler(sampler);
    }

    void WebGPUGraphicsDevice::DestroySampler(GPUSampler* sampler)
    {
        delete sampler;
    }

    GPUPipeline* WebGPUGraphicsDevice::CreateGraphicsPipeline(GPUGraphicsPipelineDesc* pipelineDesc)
    {
        if (!pipelineDesc) {
            return nullptr;
        }

        WGPURenderPipelineDescriptor renderPipelineDesc{};
        renderPipelineDesc.nextInChain = nullptr;
        renderPipelineDesc.label = {};
        renderPipelineDesc.layout = nullptr; // TODO(nemjit001): Add support for pipeline layout
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
        DynamicArray<WGPUColorTargetState> colorTargets(pipelineDesc->renderTargetCount); // FIXME(nemjit001): Use internal allocator
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
        DynamicArray<WGPUBlendState> targetBlendStates; // FIXME(nemjit001): Use internal allocator
        if (pipelineDesc->blendState)
        {
            BlendState const& pipelineBlendState = *pipelineDesc->blendState;
            targetBlendStates.resize(pipelineDesc->renderTargetCount);

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
            shaderModuleDesc.nextInChain = nullptr;
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
            shaderModuleDesc.nextInChain = nullptr;
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
        computePipelineDesc.layout = nullptr; // TODO(nemjit001): Add support for pipeline layout
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
        DynamicArray<WGPUCommandBuffer> submissions; // FIXME(nemjit001): Use internal allocator
        submissions.reserve(commandBufferCount);
        for (uint32 i = 0; i < commandBufferCount; i++)
        {
            if (commandBuffers[i]->GetBackendAPI() != BackendAPI_WebGPU) {
                return GraphicsDeviceResult_BadSubmit;
            }

            WebGPUCommandBuffer const* commandBuffer = static_cast<WebGPUCommandBuffer const*>(commandBuffers[i]);
            WGPUCommandBuffer submission = commandBuffer->GetCommandBuffer();
            if (!submission) {
                return GraphicsDeviceResult_BadSubmit;
            }

            submissions.push_back(submission);
        }

        wgpuQueueSubmit(_queue, commandBufferCount, submissions.data());
        return GraphicsDeviceResult_Ok;
    }
    
    void WebGPUGraphicsDevice::WaitIdle()
    {
        // TODO(nemjit001): Implement callback-based wait on submits (timeline semaphore?)
    }
} // namespace RHI

#endif //BONSAI_RHI_WEBGPU_SUPPORT
