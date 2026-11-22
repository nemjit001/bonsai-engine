#include "SystemRegistry.hpp"

#include <entt/entt.hpp>

namespace Bonsai
{
    void SystemRegistry::RunSystems(entt::registry& registry) const
    {
        for (auto const& system : _systems)
        {
            if (system != nullptr) {
                system->Run(registry);
            }
        }
    }

    void SystemRegistry::Clear()
    {
        _systems.clear();
    }
} // namespace Bonsai
