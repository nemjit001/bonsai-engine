#include "bonsai/subsystems/scene_manager.hpp"

namespace bonsai::subsystems
{
    bool SceneManager::init(std::optional<core::Path> const& initial_scene)
    {
        if (!SceneManager::init()) {
            return false;
        }

        return true;
    }

    bool SceneManager::init()
    {
        return IEngineSubsystem::init();
    }
} // namespace bonsai::subsystems
