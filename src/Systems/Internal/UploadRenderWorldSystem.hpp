#pragma once
#ifndef BONSAI_UPLOAD_RENDER_WORLD_SYSTEM_HPP
#define BONSAI_UPLOAD_RENDER_WORLD_SYSTEM_HPP

#include <Gfx/GraphicsTypes.hpp>
#include "Systems/ECSSystem.hpp"

namespace Bonsai
{
    class RenderManager;
    class RenderWorld;

    class UploadRenderWorldSystem : public ECSSystem
    {
    public:
        UploadRenderWorldSystem(RenderManager* renderManager);
        ~UploadRenderWorldSystem() override;

        void Run(entt::registry& registry) override final;

    private:
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

#endif //BONSAI_UPLOAD_RENDER_WORLD_SYSTEM_HPP
