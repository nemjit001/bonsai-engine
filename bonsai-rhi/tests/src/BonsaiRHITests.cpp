#include <gtest/gtest.h>
#include <BonsaiRHI.hpp>
#include <RHIShaderCompiler.hpp>

/// @brief Bonsai RHI test fixture.
class BonsaiRHITests : public testing::Test
{
public:
    static void SetUpTestSuite()
    {
        // Create a shader compiler
        _shaderCompiler = RHI::CreateShaderCompiler();
        ASSERT_NE(_shaderCompiler, nullptr);

        // Create the graphics device
        RHI::GraphicsDeviceDesc gpuDesc{};
        _gpu = RHI::CreateGraphicsDevice(&gpuDesc);
        ASSERT_NE(_gpu, nullptr);

        // Set the target shader language used by the graphics device
        _shaderCompiler->SetTargetLanguage(_gpu->GetTargetShaderLanguage());
    }

    static void TearDownTestSuite()
    {
        _gpu->WaitIdle();
        RHI::DestroyGraphicsDevice(_gpu);
        RHI::DestroyShaderCompiler(_shaderCompiler);
    }

protected:
    inline static RHI::IRHIShaderCompiler* _shaderCompiler = nullptr;
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

TEST_F(BonsaiRHITests, TestCreateEmptyDescriptorSetLayout)
{
    RHI::GPUDescriptorSetLayoutDesc setLayoutDesc{}; // Create an empty descriptor set
    RHI::GPUDescriptorSetLayout* setLayout = _gpu->CreateDescriptorSetLayout(&setLayoutDesc);
    EXPECT_NE(setLayout, nullptr);

    _gpu->DestroyDescriptorSetLayout(setLayout);
}

TEST_F(BonsaiRHITests, TestCreateDescriptorSetLayoutWithEverything)
{
    RHI::DescriptorSetLayoutBindingTextureInfo textureBindingInfo{};
    textureBindingInfo.samplerType = RHI::DescriptorSamplerType_Filtering;
    textureBindingInfo.textureSampleType = RHI::DescriptorTextureSampleType_Float;
    textureBindingInfo.textureViewType = RHI::TextureViewType_2D;
    textureBindingInfo.textureFormat = RHI::TextureFormat_RGBA32_SFloat;
    textureBindingInfo.access = RHI::DescriptorAccess_ReadWrite;
    textureBindingInfo.multisampled = false;

    RHI::DescriptorSetLayoutBinding allBindings[] = {
        RHI::DescriptorSetLayoutBinding{ 0, 1, RHI::DescriptorType_Sampler, RHI::ShaderStage_Compute, &textureBindingInfo },
        RHI::DescriptorSetLayoutBinding{ 1, 1, RHI::DescriptorType_CombinedImageSampler, RHI::ShaderStage_Compute, &textureBindingInfo },
        RHI::DescriptorSetLayoutBinding{ 2, 1, RHI::DescriptorType_SampledImage, RHI::ShaderStage_Compute, &textureBindingInfo },
        RHI::DescriptorSetLayoutBinding{ 3, 1, RHI::DescriptorType_StorageImage, RHI::ShaderStage_Compute, &textureBindingInfo },
        RHI::DescriptorSetLayoutBinding{ 4, 1, RHI::DescriptorType_UniformBuffer, RHI::ShaderStage_Compute, nullptr },
        RHI::DescriptorSetLayoutBinding{ 5, 1, RHI::DescriptorType_StorageBuffer, RHI::ShaderStage_Compute, nullptr },
        RHI::DescriptorSetLayoutBinding{ 6, 1, RHI::DescriptorType_UniformBufferDynamic, RHI::ShaderStage_Compute, nullptr },
        // RHI::DescriptorSetLayoutBinding{ 7, 1, RHI::DescriptorType_StorageBufferDynamic, RHI::ShaderStage_Compute, nullptr }, // NOTE(nemjit001): Currently unsupported on WGPU
    };

    RHI::GPUDescriptorSetLayoutDesc setLayoutDesc{};
    setLayoutDesc.bindingCount = std::size(allBindings);
    setLayoutDesc.bindings = allBindings;

    RHI::GPUDescriptorSetLayout* setLayout = _gpu->CreateDescriptorSetLayout(&setLayoutDesc);
    EXPECT_NE(setLayout, nullptr);

    _gpu->DestroyDescriptorSetLayout(setLayout);
}

TEST_F(BonsaiRHITests, TestCreatePipelineLayout)
{
    RHI::GPUPipelineLayoutDesc pipelineLayoutDesc{}; // Create an empty pipeline layout
    RHI::GPUPipelineLayout* pipelineLayout = _gpu->CreatePipelineLayout(&pipelineLayoutDesc);
    EXPECT_NE(pipelineLayout, nullptr);

    _gpu->DestroyPipelineLayout(pipelineLayout);
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

    RHI::GPUPipelineLayoutDesc pipelineLayoutDesc{};
    pipelineLayoutDesc.setLayoutCount = 0;
    pipelineLayoutDesc.setLayouts = nullptr;

    RHI::GPUPipelineLayout* pipelineLayout = _gpu->CreatePipelineLayout(&pipelineLayoutDesc);
    EXPECT_NE(pipelineLayout, nullptr);

    RHI::IShaderBlob* shaderSourceBlob = nullptr;
    EXPECT_TRUE(_shaderCompiler->CompileShaderSource(nullptr, SHADER_CODE, &shaderSourceBlob));
    ASSERT_NE(shaderSourceBlob, nullptr);

    RHI::ShaderBlob shaderBlob = {
        shaderSourceBlob->GetBufferPointer(),
        static_cast<uint32>(shaderSourceBlob->GetBufferSize()),
    };

    RHI::GPUGraphicsPipelineDesc pipelineDesc{};
    pipelineDesc.pipelineLayout = pipelineLayout;
    pipelineDesc.vertexShader = &shaderBlob;
    pipelineDesc.pixelShader = &shaderBlob;
    pipelineDesc.vertexShaderEntrypoint = "VSMain";
    pipelineDesc.pixelShaderEntrypoint = "PSMain";
    pipelineDesc.renderTargetCount = 1;
    pipelineDesc.renderTargetFormats[0] = RHI::TextureFormat_RGBA8_UNormSRGB;
    pipelineDesc.depthStencilFormat = RHI::TextureFormat_Unknown;
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
    _gpu->DestroyPipelineLayout(pipelineLayout);
    RHI::DestroyShaderBlob(shaderSourceBlob);
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

    RHI::GPUPipelineLayoutDesc pipelineLayoutDesc{};
    pipelineLayoutDesc.setLayoutCount = 0;
    pipelineLayoutDesc.setLayouts = nullptr;

    RHI::GPUPipelineLayout* pipelineLayout = _gpu->CreatePipelineLayout(&pipelineLayoutDesc);
    EXPECT_NE(pipelineLayout, nullptr);

    RHI::IShaderBlob* shaderSourceBlob = nullptr;
    EXPECT_TRUE(_shaderCompiler->CompileShaderSource("CSMain", SHADER_CODE, &shaderSourceBlob));
    ASSERT_NE(shaderSourceBlob, nullptr);

    RHI::GPUComputePipelineDesc pipelineDesc{};
    pipelineDesc.pipelineLayout = pipelineLayout;
    pipelineDesc.computeShader = { shaderSourceBlob->GetBufferPointer(), static_cast<uint32>(shaderSourceBlob->GetBufferSize()) };
    pipelineDesc.entrypoint = "CSMain";

    RHI::GPUPipeline* pipeline = _gpu->CreateComputePipeline(&pipelineDesc);
    EXPECT_NE(pipeline, nullptr);

    _gpu->DestroyPipeline(pipeline);
    _gpu->DestroyPipelineLayout(pipelineLayout);
    RHI::DestroyShaderBlob(shaderSourceBlob);
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

    // Set up descriptor set layout
    RHI::DescriptorSetLayoutBinding storageBufferLayoutBinding{};
    storageBufferLayoutBinding.binding = 0;
    storageBufferLayoutBinding.descriptorCount = 1;
    storageBufferLayoutBinding.descriptorType = RHI::DescriptorType_StorageBuffer;
    storageBufferLayoutBinding.shaderStages = RHI::ShaderStage_Compute;
    storageBufferLayoutBinding.textureBindingInfo = nullptr;

    RHI::GPUDescriptorSetLayoutDesc setLayoutDesc{};
    setLayoutDesc.bindingCount = 1;
    setLayoutDesc.bindings = &storageBufferLayoutBinding;

    RHI::GPUDescriptorSetLayout* setLayout = _gpu->CreateDescriptorSetLayout(&setLayoutDesc);
    EXPECT_NE(setLayout, nullptr);

    // Set up pipeline layout
    RHI::GPUPipelineLayoutDesc pipelineLayoutDesc{};
    pipelineLayoutDesc.setLayoutCount = 1;
    pipelineLayoutDesc.setLayouts = &setLayout;

    RHI::GPUPipelineLayout* pipelineLayout = _gpu->CreatePipelineLayout(&pipelineLayoutDesc);
    EXPECT_NE(pipelineLayout, nullptr);

    // Set up compute pipeline
    RHI::IShaderBlob* shaderSourceBlob = nullptr;
    EXPECT_TRUE(_shaderCompiler->CompileShaderSource("CSMain", SHADER_CODE, &shaderSourceBlob));
    ASSERT_NE(shaderSourceBlob, nullptr);

    RHI::GPUComputePipelineDesc pipelineDesc{};
    pipelineDesc.pipelineLayout = pipelineLayout;
    pipelineDesc.computeShader =  { shaderSourceBlob->GetBufferPointer(), static_cast<uint32>(shaderSourceBlob->GetBufferSize()) };
    pipelineDesc.entrypoint = "CSMain";

    RHI::GPUPipeline* pipeline = _gpu->CreateComputePipeline(&pipelineDesc);
    EXPECT_NE(pipeline, nullptr);
    RHI::DestroyShaderBlob(shaderSourceBlob);

    // Create output buffer and readback buffer
    RHI::GPUBufferDesc outBufferDesc{};
    outBufferDesc.heapType = RHI::GPUHeapType_DeviceLocal;
    outBufferDesc.size = sizeof(uint32) * NUM_BUFFER_ENTRIES;
    outBufferDesc.usage = RHI::BufferUsage_StorageBuffer | RHI::BufferUsage_TransferSrc;

    RHI::GPUBufferDesc readbackBufferDesc{};
    readbackBufferDesc.heapType = RHI::GPUHeapType_Readback;
    readbackBufferDesc.size = outBufferDesc.size;
    readbackBufferDesc.usage = RHI::BufferUsage_TransferDst;

    RHI::GPUBuffer* outBuffer = _gpu->CreateBuffer(&outBufferDesc);
    RHI::GPUBuffer* readbackBuffer = _gpu->CreateBuffer(&readbackBufferDesc);
    EXPECT_NE(outBuffer, nullptr);
    EXPECT_NE(readbackBuffer, nullptr);

    // Set up descriptor set
    RHI::DescriptorSetBindingBufferInfo storageBufferBindingInfo{};
    storageBufferBindingInfo.buffer = outBuffer;
    storageBufferBindingInfo.offset = 0;
    storageBufferBindingInfo.range = RHI::WHOLE_SIZE;

    RHI::DescriptorSetBinding storageBufferBinding{};
    storageBufferBinding.binding = 0;
    storageBufferBinding.arrayElementIdx = 0;
    storageBufferBinding.descriptorCount = 1;
    storageBufferBinding.descriptorType = RHI::DescriptorType_StorageBuffer;
    storageBufferBinding.bufferBindingInfo = &storageBufferBindingInfo;

    RHI::GPUDescriptorSetDesc descriptorSetDesc{};
    descriptorSetDesc.setLayout = setLayout;
    descriptorSetDesc.bindingCount = 1;
    descriptorSetDesc.bindings = &storageBufferBinding;

    RHI::GPUDescriptorSet* descriptorSet = _gpu->CreateDescriptorSet(&descriptorSetDesc);
    EXPECT_NE(descriptorSet, nullptr);

    // Record compute pass & submit
    RHI::IGPUCommandBuffer* commands = _gpu->CreateCommandBuffer(RHI::GPUQueueType_Compute);
    commands->BeginComputePass();
    commands->SetPipeline(pipeline);
    commands->BindDescriptorSets(pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
    commands->Dispatch(NUM_BUFFER_ENTRIES, 1, 1); // Linear dispatch of N workgroups
    commands->EndComputePass();
    commands->CopyBufferToBuffer(outBuffer, readbackBuffer, RHI::WHOLE_SIZE, 0, 0);
    commands->Finish();

    EXPECT_EQ(_gpu->Submit(RHI::GPUQueueType_Compute, &commands, 1), RHI::GraphicsDeviceResult_Ok);
    _gpu->DestroyCommandBuffer(commands);
    _gpu->WaitIdle();

    // Validate buffer contents
    uint32* bufferData = nullptr;
    EXPECT_EQ(_gpu->MapBuffer(readbackBuffer, 0, RHI::WHOLE_SIZE, reinterpret_cast<void**>(&bufferData)), RHI::GraphicsDeviceResult_Ok);
    EXPECT_NE(bufferData, nullptr);
    for (uint32 i = 0; i < NUM_BUFFER_ENTRIES; i++) {
        EXPECT_EQ(bufferData[i], i);
    }
    _gpu->UnmapBuffer(readbackBuffer);

    // Clean up resources
    _gpu->DestroyDescriptorSet(descriptorSet);
    _gpu->DestroyBuffer(readbackBuffer);
    _gpu->DestroyBuffer(outBuffer);
    _gpu->DestroyPipeline(pipeline);
    _gpu->DestroyPipelineLayout(pipelineLayout);
    _gpu->DestroyDescriptorSetLayout(setLayout);
}
