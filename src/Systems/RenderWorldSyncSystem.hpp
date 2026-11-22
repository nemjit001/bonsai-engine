#pragma once
#ifndef BONSAI_RENDER_WORLD_SYNC_SYSTEM_HPP
#define BONSAI_RENDER_WORLD_SYNC_SYSTEM_HPP

#include "Systems/ECSSystem.hpp"

namespace Bonsai
{
    class RenderManager;
    class RenderWorld;

    /// @brief The RenderWorldSyncSystem synchronizes the ECS scene with the render world.
    class RenderWorldSyncSystem : public ECSSystem
    {
    public:
        RenderWorldSyncSystem(RenderManager* renderManager);

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

        /// @brief Update the render world camera data.
        /// @param renderWorld Render world to update.
        /// @param registry Registry to use for camera data updates.
        void UpdateCameraData(RenderWorld& renderWorld, entt::registry const& registry);

        /// @brief Update the render world render object data.
        /// @param renderWorld Render world to update.
        /// @param registry Registry to use for render object data updates.
        void UpdateRenderObjectData(RenderWorld& renderWorld, entt::registry const& registry);

    private:
        RenderManager* _renderManager = nullptr;
    };
} // namespace Bonsai

#endif //BONSAI_RENDER_WORLD_SYNC_SYSTEM_HPP
