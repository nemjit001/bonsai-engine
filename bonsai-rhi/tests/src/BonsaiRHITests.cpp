#include <gtest/gtest.h>
#include <BonsaiRHI.hpp>

TEST(BonsaiRHITests, CreateGraphicsDevice)
{
    RHI::GraphicsDeviceDesc gpuDesc{};
    RHI::IGraphicsDevice* gpu = RHI::CreateGraphicsDevice(&gpuDesc);
    EXPECT_NE(gpu, nullptr);

    RHI::DestroyGraphicsDevice(gpu);
}
