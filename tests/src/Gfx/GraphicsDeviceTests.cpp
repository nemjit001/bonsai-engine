#include <gtest/gtest.h>
#include "Gfx/GraphicsDevice.hpp"
#include "Gfx/GraphicsTypesInternal.hpp"

class GraphicsDeviceTests : public testing::Test
{
public:
    static void SetUpTestSuite()
    {
        // Initialize SDL
        ASSERT_TRUE(SDL_Init(SDL_INIT_VIDEO));

        // Create graphics device
        _graphicsDevice = Bonsai::GraphicsDevice::TryCreate();
        ASSERT_NE(_graphicsDevice, nullptr);
    }

    static void TearDownTestSuite()
    {
        // Delete the graphics device
        delete _graphicsDevice;

        // Shut down SDL
        SDL_Quit();
    }

protected:
    inline static Bonsai::GraphicsDevice* _graphicsDevice = nullptr;
};

TEST_F(GraphicsDeviceTests, TestCreateRefcountedResource)
{
    auto resource = new Bonsai::GraphicsResource();
    ASSERT_EQ(resource->GetRefCount(), 1);
    ASSERT_EQ(resource->AddRef(), 2);
    ASSERT_EQ(resource->ReleaseRef(), 1);
    ASSERT_EQ(resource->ReleaseRef(), 0);
}

TEST_F(GraphicsDeviceTests, TestCreateGraphicsBuffer)
{
    // Attempt to create a storage buffer
    Bonsai::GraphicsBufferDesc bufferDesc{};
    bufferDesc.heapType = Bonsai::GraphicsHeapType_Default;
    bufferDesc.size = 1024u;
    bufferDesc.usage = Bonsai::GraphicsBufferUsage_TransferDst | Bonsai::GraphicsBufferUsage_StorageBuffer;

    auto buffer = _graphicsDevice->CreateBuffer(bufferDesc);
    ASSERT_NE(buffer, nullptr);

    _graphicsDevice->DestroyBuffer(buffer);
}

TEST_F(GraphicsDeviceTests, TestCreateGraphicsTextureAndView)
{
    // Attempt to create a 2D storage texture
    Bonsai::GraphicsTextureDesc textureDesc{};
    textureDesc.heapType = Bonsai::GraphicsHeapType_Default;
    textureDesc.textureType = Bonsai::GraphicsTextureType_2D;
    textureDesc.textureFormat = Bonsai::GraphicsFormat_RGBA32_SFloat;
    textureDesc.width = 1024u;
    textureDesc.height = 1024u;
    textureDesc.depthOrLayers = 1u;
    textureDesc.mipLevels = 1u;
    textureDesc.sampleCount = 1u;
    textureDesc.tilingMode = Bonsai::GraphicsTextureTiling_Optimal;
    textureDesc.usage = Bonsai::GraphicsTextureUsage_TransferDst | Bonsai::GraphicsTextureUsage_StorageImage;

    auto texture = _graphicsDevice->CreateTexture(textureDesc);
    ASSERT_NE(texture, nullptr);

    // Attempt to create a view into the texture
    Bonsai::GraphicsTextureViewDesc textureViewDesc{};
    textureViewDesc.texture = texture;
    textureViewDesc.viewType = Bonsai::GraphicsTextureViewType_2D;
    textureViewDesc.viewFormat = textureDesc.textureFormat;
    textureViewDesc.subresourceRange.aspectMask = Bonsai::GraphicsTextureAspect_Color;
    textureViewDesc.subresourceRange.baseMipLevel = 0;
    textureViewDesc.subresourceRange.levelCount = Bonsai::Graphics::RemainingMipLevels;
    textureViewDesc.subresourceRange.baseArrayLayer = 0;
    textureViewDesc.subresourceRange.layerCount = Bonsai::Graphics::RemainingArrayLayers;

    auto view = _graphicsDevice->CreateTextureView(textureViewDesc);
    ASSERT_NE(view, nullptr);

    _graphicsDevice->DestroyTexture(texture);
    _graphicsDevice->DestroyTextureView(view);
}

TEST_F(GraphicsDeviceTests, TestCreateGraphicsSampler)
{
    // Attempt to create a sampler
    Bonsai::GraphicsSamplerDesc samplerDesc{};
    samplerDesc.magFilter = Bonsai::GraphicsFilter_Linear;
    samplerDesc.minFilter = Bonsai::GraphicsFilter_Linear;
    samplerDesc.mipmapMode = Bonsai::GraphicsSamplerMipmapMode_Linear;
    samplerDesc.addressModeU = Bonsai::GraphicsSamplerAddressMode_Repeat;
    samplerDesc.addressModeV = Bonsai::GraphicsSamplerAddressMode_Repeat;
    samplerDesc.addressModeW = Bonsai::GraphicsSamplerAddressMode_Repeat;
    samplerDesc.maxAnisotropy = 0;
    samplerDesc.compareOp = Bonsai::GraphicsCompareOp_Undefined;
    samplerDesc.minLod = 0.0F;
    samplerDesc.maxLod = Bonsai::Graphics::LODClampNone;
    samplerDesc.mipLodBias = 0.0F;
    samplerDesc.borderColor = Bonsai::GraphicsBorderColor_TransparentBlack_Float;

    auto sampler = _graphicsDevice->CreateSampler(samplerDesc);
    ASSERT_NE(sampler, nullptr);

    _graphicsDevice->DestroySampler(sampler);
}

TEST_F(GraphicsDeviceTests, TestStartAndSubmitEmptyTransferBatch)
{
    auto* batch = _graphicsDevice->StartTransferBatch();
    
    // Do nothing...

    _graphicsDevice->SubmitTransferBatch(batch);
    _graphicsDevice->WaitForTransferBatch(batch);
}
