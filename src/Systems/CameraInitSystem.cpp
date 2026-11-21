#include "CameraInitSystem.hpp"

#include <Core/Logger.hpp>
#include "Components/CameraComponent.hpp"
#include "Components/TransformComponent.hpp"

namespace Bonsai
{
    void CameraInitSystem::Run(entt::registry& registry)
    {
        // Get the view into the registry
        auto view = registry.view<TransformComponent, CameraComponent>();

        // Check if a camera entity exists
        if (view.front() != entt::null) {
            return;
        }

        // Create camera entity
        entt::entity camera = registry.create();
        registry.emplace<TransformComponent>(camera, TransformComponent{});
        registry.emplace<CameraComponent>(camera, CameraComponent{});

        SPDLOG_TRACE("Spawned default camera entity");
    }
} // namespace Bonsai
