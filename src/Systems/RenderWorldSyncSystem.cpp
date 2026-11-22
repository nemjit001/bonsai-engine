#include "RenderWorldSyncSystem.hpp"

#include <cassert>
#include <Core/Logger.hpp>
#include "Renderer/RenderManager.hpp"
#include "Components/CameraComponent.hpp"
#include "Components/MeshComponent.hpp"
#include "Components/TransformComponent.hpp"

namespace Bonsai
{
    RenderWorldSyncSystem::RenderWorldSyncSystem(RenderManager* renderManager)
        :
        _renderManager(renderManager)
    {
        assert(_renderManager != nullptr && "renderManager was null!");
    }

    void RenderWorldSyncSystem::Run(entt::registry& registry)
    {
        // Get the render world
        RenderWorld& renderWorld = _renderManager->GetRenderWorld();

        // Update host side render world data
        UpdateRenderObjectCache(renderWorld, registry);
        UpdateGeometryCache(renderWorld, registry);
        UpdateCameraData(renderWorld, registry);
        UpdateRenderObjectData(renderWorld, registry);
    }

    void RenderWorldSyncSystem::UpdateRenderObjectCache(RenderWorld& renderWorld, entt::registry const& registry)
    {
        // Get relevant ECS view
        auto const renderObjectView = registry.view<TransformComponent, MeshComponent>();

        // Mark unused render objects as free, also erasing them from the object cache
        std::vector<entt::entity> eraseList;
        for (auto const& [ entity, renderObjectId ] : renderWorld.renderObjectCache)
        {
            // Check if the object still exists in the registry view
            if (renderObjectView.contains(entity)) {
                continue;
            }

            // Mark objects that no longer exist as free
            renderWorld.renderObjectFreeList.push_back(renderObjectId);
            eraseList.push_back(entity);
            SPDLOG_TRACE("Freed unused render object (entity: {} objectId: {})", entt::to_integral(entity), renderObjectId);
        }

        for (auto const& entity : eraseList) {
            renderWorld.renderObjectCache.erase(entity);
        }

        // Update render objects cache
        for (auto const& [ entity, transformComponent, meshComponent ] : renderObjectView.each())
        {
            // Check if render object already has an ID
            auto const iter = renderWorld.renderObjectCache.find(entity);
            if (iter != renderWorld.renderObjectCache.end()) {
                continue;
            }

            // Assign either a free object ID, or allocate a new render object ID
            RenderObjectId renderObjectId = 0;
            if (!renderWorld.renderObjectFreeList.empty())
            {
                renderObjectId = renderWorld.renderObjectFreeList.back();
                renderWorld.renderObjectFreeList.pop_back();
            }
            else
            {
                renderObjectId = renderWorld.maxRenderObjectId;
                renderWorld.maxRenderObjectId++;
            }

            // Insert new render object
            renderWorld.renderObjectCache.insert({ entity, renderObjectId });
            SPDLOG_TRACE("Inserted new render object (entity: {} objectId: {})", entt::to_integral(entity), renderObjectId);
        }
    }

    void RenderWorldSyncSystem::UpdateGeometryCache(RenderWorld& renderWorld, entt::registry const& registry)
    {
        // Get relevant ECS view
        auto const meshView = registry.view<MeshComponent>();

        // Insert new geometry into geometry cache
        for (auto const& [ _, meshComponent ] : meshView.each())
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

    void RenderWorldSyncSystem::UpdateCameraData(RenderWorld& renderWorld, entt::registry const& registry)
    {
        // Get relevant ECS view
        auto const cameraView = registry.view<TransformComponent, CameraComponent>();

        // Update cameras in the scene
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
    }

    void RenderWorldSyncSystem::UpdateRenderObjectData(RenderWorld& renderWorld, entt::registry const& registry)
    {
        // Get relevant ECS view
        auto const renderObjectView = registry.view<TransformComponent, MeshComponent>();

        // Resize and update render object buffers
        renderWorld.dirtyRenderObjectData.resize(renderWorld.maxRenderObjectId);
        renderWorld.renderObjects.resize(renderWorld.maxRenderObjectId);
        renderWorld.objectTransforms.resize(renderWorld.maxRenderObjectId);
        for (auto const& [ entity, transformComponent, meshComponent ] : renderObjectView.each())
        {
            // Get render object ID from cache
            RenderObjectId const& objectId = renderWorld.renderObjectCache.at(entity);

            // Update render object data
            Shader::RenderObject& renderObject = renderWorld.renderObjects[objectId];
            renderObject.objectId = objectId;

            // Update object transform
            Shader::ObjectTransform& objectTransform = renderWorld.objectTransforms[objectId];
            objectTransform.modelMatrix = transformComponent.GetMatrix();
            objectTransform.normalMatrix = glm::inverse(glm::transpose(glm::mat3(objectTransform.modelMatrix)));

            // Set dirty bit
            renderWorld.dirtyRenderObjectData[objectId] = true;
        }
    }
} // namespace Bonsai
