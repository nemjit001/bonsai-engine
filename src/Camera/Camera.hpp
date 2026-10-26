#pragma once
#ifndef BONSAI_CAMERA_HPP
#define BONSAI_CAMERA_HPP

#include <cstdint>
#include "Math/Math.hpp"

namespace Bonsai
{
    /// @brief Enumeration of different camera types.
    enum class CameraType : uint8_t
    {
        Perspective     = 0,
        Orthographic    = 1,
    };

    /// @brief Camera parameters for perspective projections.
    struct CameraPerspectiveParameters
    {
        float horizontalFOV = 60.0F;    //< Horizontal field of view in degrees.
        float aspectRatio   = 1.0F;     //< Camera aspect ratio.
        float zNear         = 0.05F;    //< Camera near plane.
        float zFar          = 1000.0F;  //< Camera far plane.
    };

    /// @brief Camera parameters for orthographic projections.
    struct CameraOrthographicParameters
    {
        float yMag          = 100.0F;   //< Size of the frustrum in the vertical direction in world units.
        float aspectRatio   = 1.0F;     //< Camera aspect ratio.
        float zNear         = 0.05F;    //< Camera near plane.
        float zFar          = 1000.0F;  //< Camera far plane.
    };

    /// @brief The Camera class implements a virtual camera.
    /// The camera supports perspective and orthographic projections.
    class Camera
    {
    public:
        Camera() = default;
        explicit Camera(CameraPerspectiveParameters const& parameters);
        explicit Camera(CameraOrthographicParameters const& parameters);
        
        /// @brief Get the camera projection matrix.
        /// @return The projection matrix for this camera, based on its camera type.
        glm::mat4 GetProjectionMatrix() const;

        /// @brief Get the type of the virtual camera.
        /// @return The virtual camera type.
        CameraType GetCameraType() const { return _cameraType; }

    private:
        CameraType _cameraType = CameraType::Perspective;
        CameraPerspectiveParameters _cameraPerspectiveParams = {};
        CameraOrthographicParameters _cameraOrthographicParams = {};
    };
} // namespace Bonsai

#endif //BONSAI_CAMERA_HPP
