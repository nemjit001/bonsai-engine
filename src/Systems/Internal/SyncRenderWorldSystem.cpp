#include "SyncRenderWorldSystem.hpp"

#include <cassert>
#include <Core/Logger.hpp>
#include <Gfx/GraphicsDevice.hpp>
#include "Renderer/RenderManager.hpp"
#include "Components/CameraComponent.hpp"
#include "Components/MeshComponent.hpp"
#include "Components/TransformComponent.hpp"

namespace Bonsai
{
    SyncRenderWorldSystem::SyncRenderWorldSystem(RenderManager* renderManager)
        :
        _renderManager(renderManager)
    {
        assert(_renderManager != nullptr && "renderManager was null!");
    }

    void SyncRenderWorldSystem::Run(entt::registry& registry)
    {
        // Get the render world
        RenderWorld& renderWorld = _renderManager->GetRenderWorld();

        // Update host side render world data
        UpdateRenderObjectCache(renderWorld, registry);
        UpdateGeometryCache(renderWorld, registry);
        UpdateSceneData(renderWorld, registry);
    }

    void SyncRenderWorldSystem::UpdateRenderObjectCache(RenderWorld& renderWorld, entt::registry const& registry)
    {
        // Get relevant ECS view
        auto const& renderObjectView = registry.view<TransformComponent, MeshComponent>();

        // Update render objects cache
        for (auto const& [ entity, transformComponent, meshComponent ] : renderObjectView.each())
        {
            // Mark existing cache entries as used
            if (renderWorld.renderObjectCache.HasRenderObjectId(entity))
            {
                renderWorld.renderObjectCache.MarkUsed(entity);
                continue;
            }

            // Assign either a free object ID, or allocate a new render object ID
            RenderObjectId const renderObjectId = renderWorld.renderObjectCache.Insert(entity);
            SPDLOG_TRACE("Inserted new render object (entity: {} objectId: {})", entt::to_integral(entity), renderObjectId);
        }

        // Update the cache, evicting unused entities
        renderWorld.renderObjectCache.Update();
    }

    void SyncRenderWorldSystem::UpdateGeometryCache(RenderWorld& renderWorld, entt::registry const& registry)
    {
        // Get relevant ECS view
        auto const& meshComponentView = registry.view<MeshComponent>();

        // Update geometry cache
        for (auto const& [ _, meshComponent ] : meshComponentView.each())
        {
            // Check if mesh has already been inserted into the geometry cache
            auto const iter = renderWorld.geometryCache.find(meshComponent.meshHandle);
            if (iter != renderWorld.geometryCache.end()) {
                continue;
            }

            // Insert new render mesh into the geometry cache
            renderWorld.geometryCache.insert({ meshComponent.meshHandle, RenderMesh{} });
            SPDLOG_TRACE("Inserted new mesh into geometry cache (asset id: {})", meshComponent.meshHandle.GetAssetId());
        }
    }

    void SyncRenderWorldSystem::UpdateSceneData(RenderWorld& renderWorld, entt::registry const& registry)
    {
        // Get relevant ECS views
        auto const& cameraView = registry.view<TransformComponent, CameraComponent>();
        auto const& renderObjectView = registry.view<TransformComponent, MeshComponent>();

        // Update cameras
        renderWorld.cameras.clear();
        for (auto const& [ entity, transformComponent, cameraComponent ] : cameraView.each())
        {
            // Get camera matrices
            glm::mat4 const viewMatrix = glm::inverse(transformComponent.GetMatrix());
            glm::mat4 const projectionMatrix = cameraComponent.camera.GetProjectionMatrix();

            // Set up camera data
            Shader::Camera camera{};
            camera.viewProjectMatrix = projectionMatrix * viewMatrix;
            renderWorld.cameras.push_back(camera);
        }

        // Update render object data
        renderWorld.renderObjects.resize(renderWorld.renderObjectCache.GetRenderObjectCount());
        renderWorld.objectTransforms.resize(renderWorld.renderObjectCache.GetRenderObjectCount());
        for (auto const& [ entity, transformComponent, meshComponent ] : renderObjectView.each())
        {
            // Get render object ID from cache
            RenderObjectId const& objectId = renderWorld.renderObjectCache.GetRenderObjectId(entity);

            // Update render object data
            Shader::RenderObject& renderObject = renderWorld.renderObjects[objectId];
            renderObject.objectId = objectId;

            // Update object transform
            Shader::ObjectTransform& objectTransform = renderWorld.objectTransforms[objectId];
            objectTransform.modelMatrix = transformComponent.GetMatrix();
            objectTransform.normalMatrix = glm::inverse(glm::transpose(glm::mat3(objectTransform.modelMatrix)));
        }
    }
} // namespace Bonsai
