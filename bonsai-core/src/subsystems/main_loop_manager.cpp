#include "bonsai/subsystems/main_loop_manager.hpp"

#include "bonsai/subsystems/engine_event_manager.hpp"
#include "bonsai/subsystems/engine_event.hpp"
#include "bonsai/subsystems/engine_subsystem.hpp"
#include "bonsai/subsystems/logging_manager.hpp"
#include "bonsai/subsystems/render_manager.hpp"
#include "bonsai/subsystems/scene_manager.hpp"
#include "bonsai/subsystems/window_manager.hpp"

namespace bonsai::subsystems
{
    MainLoopManager::MainLoopManager(
        LoggingManager* logging_manager,
        EngineEventManager* event_manager,
        WindowManager* window_manager,
        SceneManager* scene_manager,
        RenderManager* render_manager
    )
        :
        m_logging_manager(logging_manager),
        m_event_manager(event_manager),
        m_window_manager(window_manager),
        m_scene_manager(scene_manager),
        m_render_manager(render_manager)
    {
        //
    }

    bool MainLoopManager::init()
    {
        if (!IEngineSubsystem::init()) {
            return false;
        }

        m_running = true;
        return true;
    }

    void MainLoopManager::shutdown()
    {
        m_running = false;
    }

    void MainLoopManager::run()
    {
        while (m_running)
        {
            m_event_manager->run();
            m_logging_manager->run();
            m_window_manager->run();
            m_scene_manager->run();
            m_render_manager->run();
        }
    }

    void MainLoopManager::process_event(EngineEvent const& event)
    {
        switch (event.event_type)
        {
        case BONSAI_ENGINE_EVENT_TYPE_QUIT:
            m_running = false;
            break;
        default:
            break;
        }
    }
} // namespace bonsai::subsystems
