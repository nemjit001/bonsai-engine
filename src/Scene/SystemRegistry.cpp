#include "SystemRegistry.hpp"

#include <entt/entt.hpp>
#include "Scene.hpp"

namespace Bonsai
{
    void SystemRegistry::RunSystems(Scene& scene)
    {
        entt::registry& registry = scene.GetRegistry();
        for (auto const& system : _systems)
        {
            if (system != nullptr) {
                system->Run(registry);
            }
        }
    }
} // namespace Bonsai
