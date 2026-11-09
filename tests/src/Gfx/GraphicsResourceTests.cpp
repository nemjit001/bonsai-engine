#include <gtest/gtest.h>
#include "Gfx/GraphicsTypesInternal.hpp"

TEST(GraphicsResourceTests, TestCreateRefcountedResource)
{
    auto resource = new Bonsai::GraphicsResource();
    ASSERT_EQ(resource->GetRefCount(), 1);
    ASSERT_EQ(resource->AddRef(), 2);
    ASSERT_EQ(resource->ReleaseRef(), 1);
    ASSERT_EQ(resource->ReleaseRef(), 0);
}
