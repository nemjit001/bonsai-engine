#pragma once
#ifndef BONSAI_RHI_HPP
#define BONSAI_RHI_HPP

#include <BonsaiBase.hpp>
#include "BonsaiRHIExport.hpp"

namespace rhi
{
    /// @brief The GraphicsDevice interface implements a common interface on top of low-level graphics APIs.
    class BONSAI_RHI_EXPORT IGraphicsDevice
    {
    public:
        virtual ~IGraphicsDevice() = default;
    };

    /// @brief Create an RHI graphics device.
    /// @return A newly create graphics device, or nullptr on failure.
    BONSAI_RHI_EXPORT IGraphicsDevice* BONSAI_RHI_APICALL CreateGraphicsDevice();

    /// @brief Destroy an RHI graphics device.
    /// @param graphicsDevice Graphics device to destroy.
    BONSAI_RHI_EXPORT void BONSAI_RHI_APICALL DestroyGraphicsDevice(IGraphicsDevice* graphicsDevice);
} // namespace rhi

#endif //BONSAI_RHI_HPP
