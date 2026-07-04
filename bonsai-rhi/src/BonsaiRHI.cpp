#include "BonsaiRHI.hpp"

#if     BONSAI_RHI_VULKAN_SUPPORT
    // TODO(nemjit001): Include Vulkan backend
#endif  // BONSAI_RHI_VULKAN_SUPPORT

#if     BONSAI_RHI_WEBGPU_SUPPORT
    // TODO(nemjit001): Include WebGPU backend
#endif  // BONSAI_RHI_WEBGPU_SUPPORT

namespace RHI
{
    IGraphicsDevice* BONSAI_RHI_APICALL CreateGraphicsDevice(GraphicsDeviceDesc* graphicsDeviceDesc)
    {
        // Set device desc if provided
        GraphicsDeviceDesc deviceDesc{};
        if (graphicsDeviceDesc != nullptr) {
            deviceDesc = *graphicsDeviceDesc;
        }

        // Create backend graphics device
        switch (graphicsDeviceDesc->backendAPI)
        {
        case BackendAPI_Default:    // Acts as fallthrough statement to first supported backend on platform
#if     BONSAI_RHI_VULKAN_SUPPORT
        case BackendAPI_Vulkan:
            return new VulkanGraphicsDevice::TryCreate(deviceDesc);
#endif  // BONSAI_RHI_VULKAN_SUPPORT
#if     BONSAI_RHI_WEBGPU_SUPPORT
        case BackendAPI_WebGPU:
            return new WebGPUGraphicsDevice::TryCreate(deviceDesc);
#endif  // BONSAI_RHI_WEBGPU_SUPPORT
        default:
            break;
        }

        return nullptr;
    }

    void BONSAI_RHI_APICALL DestroyGraphicsDevice(IGraphicsDevice* graphicsDevice)
    {
        delete graphicsDevice;
    }
} // namespace RHI

