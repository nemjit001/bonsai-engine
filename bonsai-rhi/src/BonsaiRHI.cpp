#include "BonsaiRHI.hpp"

namespace RHI
{
    IGraphicsDevice* BONSAI_RHI_APICALL CreateGraphicsDevice([[maybe_unused]] GraphicsDeviceDesc* graphicsDeviceDesc)
    {
        return nullptr;
    }

    void BONSAI_RHI_APICALL DestroyGraphicsDevice(IGraphicsDevice* graphicsDevice)
    {
        delete graphicsDevice;
    }
} // namespace RHI

