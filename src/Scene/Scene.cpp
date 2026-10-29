#include "Scene.hpp"

#include "Systems/SystemRegistry.hpp"

namespace Bonsai
{
    void Scene::Tick(SystemRegistry const& systemRegistry)
    {
        systemRegistry.RunSystems(_registry);
    }
} // namespace Bonsai
