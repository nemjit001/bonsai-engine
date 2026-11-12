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
