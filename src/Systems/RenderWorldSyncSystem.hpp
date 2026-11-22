#pragma once
#ifndef BONSAI_RENDER_WORLD_SYNC_SYSTEM_HPP
#define BONSAI_RENDER_WORLD_SYNC_SYSTEM_HPP

#include "Systems/ECSSystem.hpp"

namespace Bonsai
{
    class RenderManager;

    /// @brief The RenderWorldSyncSystem synchronizes the ECS scene with the render world.
    class RenderWorldSyncSystem : public ECSSystem
    {
    public:
        RenderWorldSyncSystem(RenderManager* renderManager);

        void Run(entt::registry& registry) override final;

    private:
        RenderManager* _renderManager = nullptr;
    };
} // namespace Bonsai

#endif //BONSAI_RENDER_WORLD_SYNC_SYSTEM_HPP
