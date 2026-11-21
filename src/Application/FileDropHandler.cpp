#include "FileDropHandler.hpp"

#include <Core/Logger.hpp>
#include "Assets/Mesh.hpp"
#include "Components/MeshComponent.hpp"
#include "Components/RenderMeshComponent.hpp"
#include "Components/TransformComponent.hpp"

namespace Bonsai
{
    void FileDropHandler::HandleDroppedFile(Filesystem::Path const& path, AssetManager& assetManager, SceneManager& sceneManager)
    {
        if (path.extension() == ".obj")
        {
            SPDLOG_TRACE("Received a mesh asset file");

            // Load asset in asset manager
            AssetHandle<Mesh> meshHandle = assetManager.Load<Mesh>(path);
            if (!meshHandle.IsValid()) {
                SPDLOG_WARN("Failed to load mesh asset file!");
            }

            // Get the active scene and registry
            Scene& activeScene = sceneManager.GetActiveScene();
            entt::registry& registry = activeScene.GetRegistry();

            // Create entity in the scene with the new mesh handle
            entt::entity meshEntity = registry.create();
            registry.emplace<TransformComponent>(meshEntity, TransformComponent{});
            registry.emplace<MeshComponent>(meshEntity, MeshComponent{ meshHandle });
            registry.emplace<RenderMeshComponent>(meshEntity, RenderMeshComponent{});
            SPDLOG_TRACE("Spawned new mesh entity");
        }
    }
} // namespace Bonsai
