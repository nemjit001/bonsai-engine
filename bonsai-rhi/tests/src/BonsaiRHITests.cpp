#include <gtest/gtest.h>
#include <BonsaiRHI.hpp>

TEST(BonsaiRHITests, CreateGraphicsDevice)
{
    rhi::IGraphicsDevice* gpu = rhi::CreateGraphicsDevice();
    EXPECT_NE(gpu, nullptr);

    rhi::DestroyGraphicsDevice(gpu);
}
