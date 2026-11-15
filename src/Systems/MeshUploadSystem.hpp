#pragma once
#ifndef BONSAI_MESH_UPLOAD_SYSTEM_HPP
#define BONSAI_MESH_UPLOAD_SYSTEM_HPP

#include "Renderer/RenderManager.hpp"
#include "Systems/ECSSystem.hpp"

namespace Bonsai
{
    /// @brief The MeshUploadSystem handles mesh data uploads to graphics memory.
    class MeshUploadSystem : public ECSSystem
    {
    public:
        MeshUploadSystem(RenderManager* renderManager);

        void Run(entt::registry& registry) override;

    private:
        RenderManager* _renderManager = nullptr;
    };
} // namespace Bonsai

#endif //BONSAI_MESH_UPLOAD_SYSTEM_HPP
