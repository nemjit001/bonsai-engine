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
        UpdateRenderObjectUsages(registry);
        UpdateGeometryCache(registry);
    }

    void RenderWorldSyncSystem::UpdateRenderObjectUsages(entt::registry const& registry)
    {
        // Get the render world
        RenderWorld& renderWorld = _renderManager->GetRenderWorld();

        // Get relevant ECS view
        auto const& renderObjectView = registry.view<TransformComponent, MeshComponent>();

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

        // Update render objects in the render world
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
                renderObjectId = renderWorld.nextObjectId;
                renderWorld.nextObjectId++;
            }

            // Insert new render object
            renderWorld.renderObjectCache.insert({ entity, renderObjectId });
            SPDLOG_TRACE("Inserted new render object (entity: {} objectId: {})", entt::to_integral(entity), renderObjectId);
        }
    }

    void RenderWorldSyncSystem::UpdateGeometryCache(entt::registry const& registry)
    {
        // Get the render world
        RenderWorld& renderWorld = _renderManager->GetRenderWorld();

        // Get relevant ECS view
        auto const& meshView = registry.view<MeshComponent>();

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
} // namespace Bonsai
