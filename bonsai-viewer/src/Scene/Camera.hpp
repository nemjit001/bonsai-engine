#pragma once
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "BonsaiMath.hpp"
#include "SceneObject.hpp"

/// @brief A Camera represents a virtual camera in the world.
class BCamera : public BSceneObject
{
public:
    BCamera();
    BCamera(std::string const& ObjectName);

    /// @brief Get the projection matrix for the camera.
    /// @param AspectRatio Aspect ratio to use for the perspective projection.
    /// @return 
    inline glm::mat4 GetProjectionMatrix(float AspectRatio) const {
        return glm::perspective(glm::radians(HorizontalFOV), AspectRatio, ZNear, ZFar);
    }

private:
    float HorizontalFOV = 60.0F;
    float ZNear = 0.1F;
    float ZFar = 1000.0F;
};

#endif //CAMERA_HPP
