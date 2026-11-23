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

        // Set up default camera transform
        TransformComponent transformComponent{};
        transformComponent.translation = glm::vec3(0.0F, 5.0F, -5.0F);
        transformComponent.rotation = glm::quatLookAt(Math::WorldForward, Math::WorldUp);

        // Create camera entity
        entt::entity camera = registry.create();
        registry.emplace<TransformComponent>(camera, transformComponent);
        registry.emplace<CameraComponent>(camera);

        SPDLOG_TRACE("Spawned default camera entity");
    }
} // namespace Bonsai
