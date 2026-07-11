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
        wgpuCommandEncoderRelease(_commandEncoder);
    }

    void WebGPUCommandBuffer::CopyBufferToBuffer(GPUBuffer* src, GPUBuffer* dest, uint64 size, int64 srcOffset, int64 destOffset)
    {
        //
    }

    void WebGPUCommandBuffer::CopyTextureToTexture(
        GPUTexture* src, GPUTexture* dest,
        TextureSubresource srcSubresource, Offset3D srcOffset,
        TextureSubresource destSubresource, Offset3D destOffset,
        Extent3D extent
    )
    {
        //
    }

    void WebGPUCommandBuffer::CopyBufferToTexture(
        GPUBuffer* src, GPUTexture* dest,
        int64 bufferOffset, uint64 rowPitch, uint64 rowCount,
        TextureSubresource subresource, Offset3D textureOffset, Extent3D textureExtent
    )
    {
        //
    }

    void WebGPUCommandBuffer::CopyTextureToBuffer(
        GPUTexture* src, GPUBuffer* dest,
        int64 bufferOffset, uint64 rowPitch, uint64 rowCount,
        TextureSubresource subresource, Offset3D textureOffset, Extent3D textureExtent
    )
    {
        //
    }

    void WebGPUCommandBuffer::SetPipeline(GPUPipeline* pipeline)
    {
        //
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
        //
    }

    void WebGPUCommandBuffer::EndRenderPass()
    {
        //
    }

    void WebGPUCommandBuffer::Dispatch(uint32 sizeX, uint32 sizeY, uint32 sizeZ)
    {
        //
    }

    void WebGPUCommandBuffer::DispatchIndirect(GPUBuffer* dispatchCommands, int64 offset)
    {
        //
    }

    void WebGPUCommandBuffer::DrawIndexed(uint32 indexCount, uint32 instanceCount, uint32 firstIndex, int32 vertexOffset, uint32 firstInstance)
    {
        //
    }

    void WebGPUCommandBuffer::DrawIndexedIndirect(GPUBuffer* drawCommands, int64 offset)
    {
        //
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
        wgpuTextureDesc.size = {}; // TODO(nemjit001): Set up size desc
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
        wgpuSamplerDesc.maxAnisotropy = samplerDesc->maxAnisotropy;

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

    IGPUCommandBuffer* WebGPUGraphicsDevice::CreateCommandBuffer()
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


            WGPUCommandBufferDescriptor commandBufferDesc{};
            commandBufferDesc.nextInChain = nullptr;
            commandBufferDesc.label = {};

            WebGPUCommandBuffer const* commandBuffer = static_cast<WebGPUCommandBuffer const*>(commandBuffers[i]);
            WGPUCommandBuffer submission = wgpuCommandEncoderFinish(commandBuffer->GetCommandEncoder(), &commandBufferDesc);
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
