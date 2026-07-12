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
    constexpr char const* SHADER_CODE = R"(
    struct VSOutput
    {
        float4 position : SV_Position;
    };

    [shader("vertex")]
    VSOutput VSMain() {
        VSOutput result = {0};
        result.position = float4(0, 0, 0, 1);
        return result;
    }

    [shader("pixel")]
    float4 PSMain(VSOutput input) : SV_Target0 {
        return float4(0, 0, 0, 1);
    }
    )";

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
    constexpr char const* SHADER_CODE = R"(
    [shader("compute")]
    [numthreads(1, 1, 1)]
    void CSMain() {
        //
    }
    )";

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
    constexpr uint32 NUM_BUFFER_ENTRIES = 128u;
    constexpr char const* SHADER_CODE = R"(
    [[vk::binding(0, 0)]] RWStructuredBuffer<uint> gOutputBuffer : register(u0, space0);

    [shader("compute")]
    [numthreads(1, 1, 1)]
    void CSMain(uint3 threadId : SV_DispatchThreadId) {
        int bufferSize = 0;
        int stride = 0;
        gOutputBuffer.GetDimensions(bufferSize, stride);

        if (threadId.x >= bufferSize) {
            return;
        }

        gOutputBuffer[threadId.x] = threadId.x; // Write thread Id value into output buffer
    }
    )";

    RHI::GPUComputePipelineDesc pipelineDesc{};
    pipelineDesc.computeShader = {}; // FIXME(nemjit001): Implement shader compilation to req IR for gfx device
    pipelineDesc.entrypoint = "CSMain";

    // Set up compute pipeline
    RHI::GPUPipeline* pipeline = _gpu->CreateComputePipeline(&pipelineDesc);
    EXPECT_NE(pipeline, nullptr);

    // Create output buffer
    RHI::GPUBufferDesc outBufferDesc{};
    outBufferDesc.size = sizeof(uint32) * NUM_BUFFER_ENTRIES;
    outBufferDesc.usage = RHI::BufferUsage_StorageBuffer;
    RHI::GPUBuffer* outBuffer = _gpu->CreateBuffer(&outBufferDesc);
    EXPECT_NE(outBuffer, nullptr);

    // Record compute pass & submit
    RHI::IGPUCommandBuffer* commands = _gpu->CreateCommandBuffer(RHI::GPUQueueType_Compute);
    commands->SetPipeline(pipeline); // TODO(nemjit001): bind params some way or another
    commands->Dispatch(NUM_BUFFER_ENTRIES, 1, 1); // Linear dispatch of N workgroups
    commands->Finish();

    EXPECT_EQ(_gpu->Submit(RHI::GPUQueueType_Compute, &commands, 1), RHI::GraphicsDeviceResult_Ok);
    _gpu->DestroyCommandBuffer(commands);

    // Validate buffer contents
    uint32* bufferData = nullptr;
    EXPECT_EQ(_gpu->MapBuffer(outBuffer, 0, RHI::WHOLE_SIZE, reinterpret_cast<void**>(&bufferData)), RHI::GraphicsDeviceResult_Ok);
    EXPECT_NE(bufferData, nullptr);
    for (uint32 i = 0; i < NUM_BUFFER_ENTRIES; i++) {
        EXPECT_EQ(bufferData[i], i);
    }

    _gpu->UnmapBuffer(outBuffer);
    _gpu->DestroyBuffer(outBuffer);
    _gpu->DestroyPipeline(pipeline);
}
