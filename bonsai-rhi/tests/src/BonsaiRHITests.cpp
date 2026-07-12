#include <gtest/gtest.h>
#include <BonsaiRHI.hpp>

/// @brief Bonsai RHI test fixture.
class BonsaiRHITests : public testing::Test
{
public:
    static void SetUpTestSuite()
    {
        RHI::GraphicsDeviceDesc gpuDesc{};
        _gpu = RHI::CreateGraphicsDevice(&gpuDesc);
        ASSERT_NE(_gpu, nullptr);
    }

    static void TearDownTestSuite()
    {
        _gpu->WaitIdle();
        RHI::DestroyGraphicsDevice(_gpu);
    }

protected:
    inline static RHI::IGraphicsDevice* _gpu = nullptr;
};

TEST_F(BonsaiRHITests, TestCreateGraphicsDevice)
{
    EXPECT_NE(_gpu, nullptr);
}

TEST_F(BonsaiRHITests, TestCreateBuffer)
{
    RHI::GPUBufferDesc bufferDesc{};
    bufferDesc.heapType = RHI::GPUHeapType_Default;
    bufferDesc.size = 512;
    bufferDesc.usage = RHI::BufferUsage_TransferSrc;

    RHI::GPUBuffer* buffer = _gpu->CreateBuffer(&bufferDesc);
    EXPECT_NE(buffer, nullptr);

    _gpu->DestroyBuffer(buffer);
}

TEST_F(BonsaiRHITests, TestCreateTextureAndView)
{
    RHI::GPUTextureDesc textureDesc{};
    textureDesc.textureType = RHI::TextureType_2D;
    textureDesc.format = RHI::TextureFormat_RGBA32_SFloat;
    textureDesc.size = { 512, 512, 1 };
    textureDesc.mipLevels = 1;
    textureDesc.arrayLayers = 1;
    textureDesc.sampleCount = 1;
    textureDesc.usage = RHI::TextureUsage_Storage;

    RHI::GPUTexture* texture = _gpu->CreateTexture(&textureDesc);
    EXPECT_NE(texture, nullptr);

    RHI::GPUTextureViewDesc viewDesc{};
    viewDesc.texture = texture;
    viewDesc.viewType = RHI::TextureViewType_2D;
    viewDesc.format = RHI::TextureFormat_RGBA32_SFloat;
    viewDesc.subresourceRange.aspectMask = RHI::TextureAspect_Color;
    viewDesc.subresourceRange.baseMipLevel = 0;
    viewDesc.subresourceRange.levelCount = RHI::ALL_REMAINING_MIPS;
    viewDesc.subresourceRange.baseArrayLayer = 0;
    viewDesc.subresourceRange.layerCount = RHI::ALL_REMAINING_LAYERS;

    RHI::GPUTexture* view = _gpu->CreateTextureView(&viewDesc);
    EXPECT_NE(view, nullptr);

    _gpu->DestroyTexture(view);
    _gpu->DestroyTexture(texture);
}

TEST_F(BonsaiRHITests, TestCreateSampler)
{
    RHI::GPUSamplerDesc samplerDesc{}; // Just use sane defaults
    RHI::GPUSampler* sampler = _gpu->CreateSampler(&samplerDesc);
    EXPECT_NE(sampler, nullptr);

    _gpu->DestroySampler(sampler);
}

TEST_F(BonsaiRHITests, TestCreateGraphicsPipeline)
{
    // FIXME(nemjit001): Implement shader compilation to req IR for gfx device
    RHI::ShaderBlob shaderBlob = {};

    RHI::GPUGraphicsPipelineDesc pipelineDesc{};
    pipelineDesc.vertexShader = &shaderBlob;
    pipelineDesc.pixelShader = &shaderBlob;
    pipelineDesc.vertexShaderEntrypoint = "VSMain";
    pipelineDesc.pixelShaderEntrypoint = "PSMain";
    pipelineDesc.renderTargetCount = 1;
    pipelineDesc.renderTargetFormats[0] = RHI::TextureFormat_Unknown;
    pipelineDesc.depthStencilFormat = RHI::TextureFormat_RGBA8_UNormSRGB;
    pipelineDesc.rasterState.polygonMode = RHI::PolygonMode_Fill;
    pipelineDesc.rasterState.primitiveTopology = RHI::PrimitiveTopology_TriangleList;
    pipelineDesc.rasterState.cullMode = RHI::CullMode_None;
    pipelineDesc.rasterState.frontFaceCounterClockwise = false;
    pipelineDesc.rasterState.sampleCount = 1;
    pipelineDesc.rasterState.alphaToCoverageEnable = false;
    pipelineDesc.rasterState.alphaToOneEnable = false;
    pipelineDesc.depthStencilState = nullptr; // No depth/stencil state
    pipelineDesc.blendState = nullptr; // No blend state

    RHI::GPUPipeline* pipeline = _gpu->CreateGraphicsPipeline(&pipelineDesc);
    EXPECT_NE(pipeline, nullptr);

    _gpu->DestroyPipeline(pipeline);
}

TEST_F(BonsaiRHITests, TestCreateComputePipeline)
{
    RHI::GPUComputePipelineDesc pipelineDesc{};
    pipelineDesc.computeShader = {}; // FIXME(nemjit001): Implement shader compilation to req IR for gfx device
    pipelineDesc.entrypoint = "CSMain";

    RHI::GPUPipeline* pipeline = _gpu->CreateComputePipeline(&pipelineDesc);
    EXPECT_NE(pipeline, nullptr);

    _gpu->DestroyPipeline(pipeline);
}

TEST_F(BonsaiRHITests, TestMapBufferRegion)
{
    RHI::GPUBufferDesc bufferDesc{};
    bufferDesc.heapType = RHI::GPUHeapType_Default;
    bufferDesc.size = sizeof(uint32);
    bufferDesc.usage = RHI::BufferUsage_TransferSrc;

    RHI::GPUBuffer* buffer = _gpu->CreateBuffer(&bufferDesc);
    EXPECT_NE(buffer, nullptr);

    uint32* bufferData = nullptr;
    EXPECT_EQ(_gpu->MapBuffer(buffer, 0, sizeof(uint32), reinterpret_cast<void**>(&bufferData)), RHI::GraphicsDeviceResult_Ok);
    EXPECT_NE(bufferData, nullptr);

    _gpu->UnmapBuffer(buffer);
    _gpu->DestroyBuffer(buffer);
}

TEST_F(BonsaiRHITests, TestMapReadbackBufferRegion)
{
    RHI::GPUBufferDesc bufferDesc{};
    bufferDesc.heapType = RHI::GPUHeapType_Readback;
    bufferDesc.size = sizeof(uint32);
    bufferDesc.usage = RHI::BufferUsage_TransferDst;

    RHI::GPUBuffer* buffer = _gpu->CreateBuffer(&bufferDesc);
    EXPECT_NE(buffer, nullptr);

    uint32* bufferData = nullptr;
    EXPECT_EQ(_gpu->MapBuffer(buffer, 0, sizeof(uint32), reinterpret_cast<void**>(&bufferData)), RHI::GraphicsDeviceResult_Ok);
    EXPECT_NE(bufferData, nullptr);

    _gpu->UnmapBuffer(buffer);
    _gpu->DestroyBuffer(buffer);
}

TEST_F(BonsaiRHITests, TestDoOffscreenComputeSubmit)
{
    // Set up basic compute shader that fills an output buffer
    // FIXME(nemjit001): Set up basic compute shader with a expected output (e.g. a simple fill to fixed value?)
    RHI::GPUComputePipelineDesc pipelineDesc{};
    pipelineDesc.computeShader = {}; // FIXME(nemjit001): Implement shader compilation to req IR for gfx device
    pipelineDesc.entrypoint = "CSMain";

    // Set up compute pipeline
    RHI::GPUPipeline* pipeline = _gpu->CreateComputePipeline(&pipelineDesc);
    EXPECT_NE(pipeline, nullptr);

    // Create output buffer
    RHI::GPUBufferDesc outBufferDesc{};
    outBufferDesc.size = sizeof(uint32) * 32;
    outBufferDesc.usage = RHI::BufferUsage_StorageBuffer;
    RHI::GPUBuffer* outBuffer = _gpu->CreateBuffer(&outBufferDesc);
    EXPECT_NE(outBuffer, nullptr);

    // Record compute pass & submit
    RHI::IGPUCommandBuffer* commands = _gpu->CreateCommandBuffer(RHI::GPUQueueType_Compute);
    commands->SetPipeline(pipeline); // TODO(nemjit001): bind params some way or another
    commands->Dispatch(1, 1, 1);
    commands->Finish();

    EXPECT_EQ(_gpu->Submit(RHI::GPUQueueType_Compute, &commands, 1), RHI::GraphicsDeviceResult_Ok);
    _gpu->DestroyCommandBuffer(commands);

    // Validate buffer contents
    uint32* bufferData = nullptr;
    EXPECT_EQ(_gpu->MapBuffer(outBuffer, 0, sizeof(uint32) * 32, reinterpret_cast<void**>(&bufferData)), RHI::GraphicsDeviceResult_Ok);
    EXPECT_NE(bufferData, nullptr);
    for (uint32 i = 0; i < 32; i++) {
        EXPECT_EQ(bufferData[i], UINT32_MAX);
    }

    _gpu->UnmapBuffer(outBuffer);
    _gpu->DestroyBuffer(outBuffer);
    _gpu->DestroyPipeline(pipeline);
}
