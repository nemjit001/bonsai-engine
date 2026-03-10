#include <gtest/gtest.h>
#include "Renderer.hpp"

struct MockResource
{
    int Value;
};

TEST(RendererTests, TestResourcePool)
{
    TRenderResourcePool<MockResource> ResourcePool;
    TRenderHandle<MockResource> Handle = ResourcePool.Allocate(MockResource{ 0x42 });

    MockResource Resource{};
    ASSERT_TRUE(ResourcePool.GetResource(Handle, Resource));
    ASSERT_EQ(Resource.Value, 0x42);

    ResourcePool.Free(Handle);
    ASSERT_FALSE(ResourcePool.IsValidResource(Handle));
}
