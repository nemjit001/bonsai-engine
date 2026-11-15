#pragma once
#ifndef BONSAI_CAMERA_COMPONENT_HPP
#define BONSAI_CAMERA_COMPONENT_HPP

#include "Gfx/GraphicsDevice.hpp"
#include "Camera/Camera.hpp"

namespace Bonsai
{
    /// @brief The CameraComponent attaches a virtual camera to an entity, to be used for rendering.
    /// It contains the camera parameters and related graphics resources.
    struct CameraComponent
    {
        Camera camera = Camera();        
        GraphicsBuffer* cameraConstantBuffer = nullptr;
    };
} // namespace Bonsai

#endif //BONSAI_CAMERA_COMPONENT_HPP
