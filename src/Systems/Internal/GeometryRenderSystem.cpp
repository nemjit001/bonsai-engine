#include "GeometryRenderSystem.hpp"

#include <cassert>
#include "Renderer/RenderManager.hpp"
#include "Components/CameraComponent.hpp"
#include "Components/MeshComponent.hpp"
#include "Components/TransformComponent.hpp"

namespace Bonsai
{
    GeometryRenderSystem::GeometryRenderSystem(RenderManager* renderManager)
        :
        _renderManager(renderManager)
    {
        assert(_renderManager != nullptr && "renderManager was null!");
    }

    void GeometryRenderSystem::Run(entt::registry& registry)
    {
        // Get render world
        RenderWorld& renderWorld = _renderManager->GetRenderWorld();

        // Get ECS views
        auto const& cameraView = registry.view<TransformComponent, CameraComponent>();
        auto const& renderObjectView = registry.view<TransformComponent, MeshComponent>();

        // Clear render queues
        renderWorld.geometryRenderQueue.clear();

        // Collect per-camera draw lists
        uint64_t cameraId = 0;
        for (auto const& _ : cameraView.each())
        {
            for (auto const& [ entity, _, meshComponent ] : renderObjectView.each())
            {
                // Get mesh and render mesh from mesh handle
                auto const mesh = meshComponent.meshHandle.Get();
                RenderMesh const renderMesh = renderWorld.geometryCache.at(meshComponent.meshHandle);

                // Get render objectID
                RenderObjectId const objectId = renderWorld.renderObjectCache.GetRenderObjectId(entity);

                // Iterate over sub meshes and collect draw calls
                uint64_t meshId = 0;
                for (auto const& subMesh : mesh->GetSubmeshes())
                {
                    // Create geometry draw command
                    GeometryDrawCommand drawCommand{};
                    drawCommand.sortKey.fields.cameraId = cameraId;
                    drawCommand.sortKey.fields.objectId = objectId;
                    drawCommand.sortKey.fields.meshId = meshId;
                    drawCommand.renderMesh = renderMesh;
                    drawCommand.subMesh = subMesh;

                    
                    renderWorld.geometryRenderQueue.push_back(drawCommand);
                    meshId++;
                }
            }

            cameraId++;
        }

        // Sort draw commands by sort key
        std::sort(
            renderWorld.geometryRenderQueue.begin(),
            renderWorld.geometryRenderQueue.end(),
            [](GeometryDrawCommand const& a, GeometryDrawCommand const& b) {
                return a.sortKey.value < b.sortKey.value;
            }
        );
    }
} // namespace Bonsai
