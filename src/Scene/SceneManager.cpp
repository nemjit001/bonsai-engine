#include "SceneManager.hpp"

namespace Bonsai
{
    bool SceneManager::Initialize()
    {
        return true;
    }

    void SceneManager::Shutdown()
    {
        // Clear active scene
        _activeScene.Clear();
    }
} // namespace Bonsai
