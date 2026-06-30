#include "BonsaiRHI.hpp"

namespace rhi
{
    IGraphicsDevice* BONSAI_RHI_APICALL CreateGraphicsDevice()
    {
        return nullptr;
    }

    void BONSAI_RHI_APICALL DestroyGraphicsDevice(IGraphicsDevice* graphicsDevice)
    {
        delete graphicsDevice;
    }
} // namespace rhi

