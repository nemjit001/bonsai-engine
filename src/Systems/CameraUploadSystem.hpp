#pragma once
#ifndef BONSAI_CAMERA_UPLOAD_SYSTEM_HPP
#define BONSAI_CAMERA_UPLOAD_SYSTEM_HPP

#include "Renderer/RenderManager.hpp"
#include "Systems/ECSSystem.hpp"

namespace Bonsai
{
    /// @brief The CameraUploadSystem handles camera data uploads to graphics memory.
    class CameraUploadSystem : public ECSSystem
    {
    public:
        CameraUploadSystem(RenderManager* renderManager);

        void Run(entt::registry& registry) override;

    private:
        RenderManager* _renderManager = nullptr;
    };
} // namespace Bonsai

#endif //BONSAI_CAMERA_UPLOAD_SYSTEM_HPP
