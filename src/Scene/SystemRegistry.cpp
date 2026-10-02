#include "SystemRegistry.hpp"

namespace Bonsai
{
    void SystemRegistry::RunSystems(entt::registry& registry)
    {
        for (auto const& system : _systems)
        {
            if (system != nullptr) {
                system->Run(registry);
            }
        }
    }
} // namespace Bonsai
