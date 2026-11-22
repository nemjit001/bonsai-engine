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
        /// @brief Update the render world render object usages.
        /// @param registry Registry to use for tracking render object usages.
        void UpdateRenderObjectUsages(entt::registry const& registry);

        /// @brief Update the render world geometry cache.
        /// @param registry Registry to use for geometry cache updates.
        void UpdateGeometryCache(entt::registry const& registry);

    private:
        RenderManager* _renderManager = nullptr;
    };
} // namespace Bonsai

#endif //BONSAI_RENDER_WORLD_SYNC_SYSTEM_HPP
