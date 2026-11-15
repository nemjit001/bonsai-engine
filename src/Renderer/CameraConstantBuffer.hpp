#pragma once
#ifndef BONSAI_CAMERA_CONSTANCT_BUFFER_HPP
#define BONSAI_CAMERA_CONSTANCT_BUFFER_HPP

#include "Math/Math.hpp"

namespace Bonsai
{
    /// @brief The CameraConstantBuffer is a shader type that contains camera data.
    struct CameraConstantBuffer
    {
        alignas(16) glm::mat4 viewProjectMatrix;
    };
} // namespace Bonsai

#endif //BONSAI_CAMERA_CONSTANCT_BUFFER_HPP
