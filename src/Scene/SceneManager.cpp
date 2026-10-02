#include "SceneManager.hpp"

namespace Bonsai
{
    void SceneManager::Tick()
    {
        _systemRegistry.RunSystems(_activeScene);
    }
} // namespace Bonsai
