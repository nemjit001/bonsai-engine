#pragma once
#ifndef BONSAI_GEOMETRY_RENDER_SYSTEM_HPP
#define BONSAI_GEOMETRY_RENDER_SYSTEM_HPP

#include "Systems/ECSSystem.hpp"

namespace Bonsai
{
    class RenderManager;

    /// @brief The GeometryRenderSystem collects high level geometry draw data for dispatch to a render queue.
    class GeometryRenderSystem : public ECSSystem
    {
    public:
        GeometryRenderSystem(RenderManager* renderManager);

        void Run(entt::registry& registry) override final;

    private:
        RenderManager* _renderManager = nullptr;
    };
} // namespace Bonsai

#endif //BONSAI_GEOMETRY_RENDER_SYSTEM_HPP
