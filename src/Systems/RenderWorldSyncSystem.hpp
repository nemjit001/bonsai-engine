#pragma once
#ifndef BONSAI_RENDER_WORLD_SYNC_SYSTEM_HPP
#define BONSAI_RENDER_WORLD_SYNC_SYSTEM_HPP

#include <Gfx/GraphicsTypes.hpp>
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
        ~RenderWorldSyncSystem() override;

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

        /// @brief Upload the render world geometry to device memory.
        /// @param renderWorld Render world to use for upload.
        void UploadGeometry(RenderWorld& renderWorld);

        /// @brief Upload the render world scene data to device memory.
        /// @param renderWorld Render world to use for upload.
        void UploadSceneData(RenderWorld& renderWorld);

    private:
        /// @brief Increment size for upload buffer reallocations
        static constexpr size_t UploadBufferIncrementSize = 4096;

        RenderManager* _renderManager = nullptr;
        size_t _sceneDataUploadBufferSize = 0;
        GraphicsBuffer* _sceneDataUploadBuffer = nullptr;
    };
} // namespace Bonsai

#endif //BONSAI_RENDER_WORLD_SYNC_SYSTEM_HPP
