#pragma once
#ifndef BONSAI_SYNC_RENDER_WORLD_SYSTEM_HPP
#define BONSAI_SYNC_RENDER_WORLD_SYSTEM_HPP

#include <Gfx/GraphicsTypes.hpp>
#include "Systems/ECSSystem.hpp"

namespace Bonsai
{
    class RenderManager;
    class RenderWorld;

    /// @brief The SyncRenderWorldSystem synchronizes the host-side render world with the ECS scene.
    class SyncRenderWorldSystem : public ECSSystem
    {
    public:
        SyncRenderWorldSystem(RenderManager* renderManager);

        void Run(entt::registry& registry) override final;

    private:
        /// @brief Update the render world render object cache.
        /// @param renderWorld Render world to update.
        /// @param registry Registry to use for tracking render objects.
        void UpdateRenderObjectCache(RenderWorld& renderWorld, entt::registry const& registry);

        /// @brief Update the render world geometry cache.
        /// @param renderWorld Render world to update.
        /// @param registry Registry to use for geometry cache updates.
        void UpdateGeometryCache(RenderWorld& renderWorld, entt::registry const& registry);

        /// @brief Update the render world scene data.
        /// @param renderWorld Render world to update.
        /// @param registry Registry to use for scene data updates.
        void UpdateSceneData(RenderWorld& renderWorld, entt::registry const& registry);

    private:
        RenderManager* _renderManager = nullptr;
    };
} // namespace Bonsai

#endif //BONSAI_RENDER_WORLD_SYNC_SYSTEM_HPP
