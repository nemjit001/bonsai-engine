#include "Camera.hpp"

namespace Bonsai
{
    Camera::Camera(CameraPerspectiveParameters const& parameters)
        :
        _cameraType(CameraType::Perspective),
        _cameraPerspectiveParams(parameters)
    {
        //
    }

    Camera::Camera(CameraOrthographicParameters const& parameters)
        :
        _cameraType(CameraType::Orthographic),
        _cameraOrthographicParams(parameters)
    {
        //
    }

    void Camera::SetAspectRatio(float aspectRatio)
    {
        if (_cameraType == CameraType::Perspective)
        {
            _cameraPerspectiveParams.aspectRatio = aspectRatio;
        }

        if (_cameraType == CameraType::Perspective)
        {
            _cameraOrthographicParams.aspectRatio = aspectRatio;
        }
    }

    glm::mat4 Camera::GetProjectionMatrix() const
    {
        if (_cameraType == CameraType::Perspective)
        {
            return glm::perspective(
                glm::radians(_cameraPerspectiveParams.horizontalFOV),
                _cameraPerspectiveParams.aspectRatio,
                _cameraPerspectiveParams.zNear,
                _cameraPerspectiveParams.zFar
            );
        }
        
        if (_cameraType == CameraType::Orthographic)
        {
            float const xMag = _cameraOrthographicParams.yMag * _cameraOrthographicParams.aspectRatio;
            return glm::ortho(
                -0.5F * xMag,
                0.5F * xMag,
                -0.5F * _cameraOrthographicParams.yMag,
                0.5F * _cameraOrthographicParams.yMag,
                _cameraOrthographicParams.zNear,
                _cameraOrthographicParams.zFar
            );
        }

        return glm::identity<glm::mat4>();
    }
} // namespace Bonsai
