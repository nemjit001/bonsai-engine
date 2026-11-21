#include "GraphicsDevice.hpp"

#include "Gfx/Vulkan/VulkanGraphicsDevice.hpp"

namespace Bonsai
{
    GraphicsDevice* GraphicsDevice::TryCreate()
    {
        return VulkanGraphicsDevice::TryCreate();
    }

    void GraphicsDevice::Destroy(GraphicsDevice* graphicsDevice)
    {
        delete graphicsDevice;
    }
} // namespace Bonsai
