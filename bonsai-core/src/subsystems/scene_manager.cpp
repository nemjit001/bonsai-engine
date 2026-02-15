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
            std::optional<assets::Scene> loaded_scene = assets::AssetLoader<assets::Scene>::load(*initial_scene);
            if (loaded_scene.has_value())
            {
                *m_scene = std::move(*loaded_scene); // Moves loaded scene into value stored in pointer
            }
        }

        return true;
    }

    void SceneManager::shutdown()
    {
        m_scene.release();
    }

    void SceneManager::run()
    {
        // TODO(nemjit001): Update active scene state by iteratively running ECS systems through ECS manager
    }

    bool SceneManager::init()
    {
        if (!IEngineSubsystem::init()) {
            return false;
        }

        m_scene = std::make_unique<assets::Scene>();
        return true;
    }
} // namespace bonsai::subsystems
