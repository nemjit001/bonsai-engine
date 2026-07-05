#include "WebGPUGraphicsDevice.hpp"
#if BONSAI_RHI_WEBGPU_SUPPORT

#include "WebGPUTypes.hpp"

namespace RHI
{
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

    WebGPUGraphicsDevice::WebGPUGraphicsDevice(WGPUInstance instance, WGPUAdapter adapter, WGPUDevice device, uint32 framesInFlight)
        :
        _instance(instance),
        _adapter(adapter),
        _device(device),
        _queue(wgpuDeviceGetQueue(device)),
        _framesInFlight(framesInFlight)
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

    RHI::IGraphicsDevice* WebGPUGraphicsDevice::TryCreate(RHI::GraphicsDeviceDesc const& graphicsDeviceDesc)
    {
        // Validate params
        if (graphicsDeviceDesc.framesInFlight == 0) {
            return nullptr;
        }

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

        return new WebGPUGraphicsDevice(instance, adapter, device, graphicsDeviceDesc.framesInFlight);
    }

    GPUBuffer* WebGPUGraphicsDevice::CreateBuffer(GPUBufferDesc* bufferDesc)
    {
        if (!bufferDesc) {
            return nullptr;
        }
        
        WGPUBufferUsage usage = 0;
        if (BitFlagIsSet(bufferDesc->usage, BufferUsage_TransferSrc)) {
            usage |= WGPUBufferUsage_CopySrc;
        }
        if (BitFlagIsSet(bufferDesc->usage, BufferUsage_TransferDst)) {
            usage |= WGPUBufferUsage_CopyDst;
        }
        if (BitFlagIsSet(bufferDesc->usage, BufferUsage_UniformBuffer)) {
            usage |= WGPUBufferUsage_Uniform;
        }
        if (BitFlagIsSet(bufferDesc->usage, BufferUsage_StorageBuffer)) {
            usage |= WGPUBufferUsage_Storage;
        }
        if (BitFlagIsSet(bufferDesc->usage, BufferUsage_IndexBuffer)) {
            usage |= WGPUBufferUsage_Index;
        }
        if (BitFlagIsSet(bufferDesc->usage, BufferUsage_VertexBuffer)) {
            usage |= WGPUBufferUsage_Vertex;
        }
        if (BitFlagIsSet(bufferDesc->usage, BufferUsage_IndirectBuffer)) {
            usage |= WGPUBufferUsage_Indirect;
        }

        WGPUBufferDescriptor wgpuBufferDesc{};
        wgpuBufferDesc.nextInChain = nullptr;
        wgpuBufferDesc.label = {};
        wgpuBufferDesc.usage = usage;
        wgpuBufferDesc.size = bufferDesc->size;
        wgpuBufferDesc.mappedAtCreation = false;

        WGPUBuffer buffer = wgpuDeviceCreateBuffer(_device, &wgpuBufferDesc);
        if (!buffer) {
            return nullptr;
        }

        return new WebGPUBuffer(BackendAPI_WebGPU, buffer);
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

        WGPUTextureUsage usage = 0;
        if (BitFlagIsSet(textureDesc->usage, TextureUsage_TransferSrc)) {
            usage |= WGPUTextureUsage_CopySrc;
        }
        if (BitFlagIsSet(textureDesc->usage, TextureUsage_TransferDst)) {
            usage |= WGPUTextureUsage_CopyDst;
        }
        if (BitFlagIsSet(textureDesc->usage, TextureUsage_Sampled)) {
            usage |= WGPUTextureUsage_TextureBinding;
        }
        if (BitFlagIsSet(textureDesc->usage, TextureUsage_Storage)) {
            usage |= WGPUTextureUsage_StorageBinding;
        }
        if (BitFlagIsSet(textureDesc->usage, TextureUsage_RenderTarget)
            || BitFlagIsSet(textureDesc->usage, TextureUsage_DepthStencilTarget)) {
            usage |= WGPUTextureUsage_RenderAttachment;
        }

        WGPUTextureDescriptor wgpuTextureDesc{};
        wgpuTextureDesc.nextInChain = nullptr;
        wgpuTextureDesc.label = {};
        wgpuTextureDesc.usage = usage;
        wgpuTextureDesc.dimension = WGPUTextureDimension_1D; // TODO(nemjit001): use LUT for dims
        wgpuTextureDesc.size = {}; // TODO(nemjit001): Set up size desc
        wgpuTextureDesc.format = WGPUTextureFormat_Undefined; // TODO(nemjit001): use LUT for format
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

        return new WebGPUTexture(BackendAPI_WebGPU, texture, view);
    }

    GPUTexture* WebGPUGraphicsDevice::CreateTextureView(GPUTextureViewDesc* textureViewDesc)
    {
        return nullptr;
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
        wgpuSamplerDesc.addressModeU = WGPUAddressMode_Undefined; // TODO(nemjit001): use LUT for address mode
        wgpuSamplerDesc.addressModeV = WGPUAddressMode_Undefined;
        wgpuSamplerDesc.addressModeW = WGPUAddressMode_Undefined;
        wgpuSamplerDesc.magFilter = WGPUFilterMode_Nearest; // TODO(nemjit001): use LUT for filter mode
        wgpuSamplerDesc.minFilter = WGPUFilterMode_Nearest;
        wgpuSamplerDesc.mipmapFilter = WGPUMipmapFilterMode_Nearest; // TODO(nemjit001): use LUT for mipmap mode
        wgpuSamplerDesc.lodMinClamp = samplerDesc->minLOD;
        wgpuSamplerDesc.lodMaxClamp = samplerDesc->maxLOD;
        wgpuSamplerDesc.compare = samplerDesc->enableCompareOp ?
            WGPUCompareFunction_Never : WGPUCompareFunction_Undefined; // TODO(nemjit001): use LUT for compare op
        wgpuSamplerDesc.maxAnisotropy = samplerDesc->maxAnisotropy;

        WGPUSampler sampler = wgpuDeviceCreateSampler(_device, &wgpuSamplerDesc);
        if (!sampler) {
            return nullptr;
        }

        return new WebGPUSampler(BackendAPI_WebGPU, sampler);
    }

    void WebGPUGraphicsDevice::DestroySampler(GPUSampler* sampler)
    {
        delete sampler;
    }

    GPUPipeline* WebGPUGraphicsDevice::CreateGraphicsPipeline(GPUGraphicsPipelineDesc* pipelineDesc)
    {
        return nullptr;
    }

    GPUPipeline* WebGPUGraphicsDevice::CreateComputePipeline(GPUComputePipelineDesc* pipelineDesc)
    {
        if (!pipelineDesc) {
            return nullptr;
        }

        WGPUShaderSourceWGSL computeShaderSource{};
        computeShaderSource.chain.sType = WGPUSType_ShaderSourceWGSL;
        computeShaderSource.chain.next = nullptr;
        computeShaderSource.code.data = static_cast<char*>(pipelineDesc->computeShader.code);
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
        computePipelineDesc.layout = nullptr;
        computePipelineDesc.compute.nextInChain = nullptr;
        computePipelineDesc.compute.module = computeShader;
        computePipelineDesc.compute.entryPoint = {}; // TODO(nemjit001): Provide entrypoint name in desc
        computePipelineDesc.compute.constantCount = 0;
        computePipelineDesc.compute.constants = nullptr;

        WGPUComputePipeline pipeline = wgpuDeviceCreateComputePipeline(_device, &computePipelineDesc);
        wgpuShaderModuleRelease(computeShader);
        
        if (!pipeline) {
            return nullptr;
        }

        return new WebGPUPipeline(BackendAPI_WebGPU, pipeline);
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

    GraphicsDeviceResult WebGPUGraphicsDevice::Submit(GPUQueueType queue, IGPUCommandBuffer** commandBuffers, uint32 commandBufferCount)
    {
        return GraphicsDeviceResult_Ok;
    }
    
    void WebGPUGraphicsDevice::WaitIdle()
    {
        //
    }
} // namespace RHI

#endif //BONSAI_RHI_WEBGPU_SUPPORT
