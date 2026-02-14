#include "bonsai/subsystems/scene_manager.hpp"

namespace bonsai::subsystems
{
    bool SceneManager::init(std::optional<core::Path> const& initial_scene)
    {
        if (!SceneManager::init()) {
            return false;
        }

        if (initial_scene.has_value())
        {
            // TODO(nemjit001): Load scene asset from disk
        }
        else
        {
            // TODO(nemjit001): Initialize empty scene asset
        }

        return true;
    }

    void SceneManager::shutdown()
    {
        // TODO(nemjit001): Unload active scene from memory
    }

    void SceneManager::run()
    {
        // TODO(nemjit001): Update active scene state by iteratively running ECS systems through ECS manager
    }

    bool SceneManager::init()
    {
        return IEngineSubsystem::init();
    }
} // namespace bonsai::subsystems
