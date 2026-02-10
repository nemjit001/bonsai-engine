#include "bonsai/subsystems/main_loop_manager.hpp"

#include "bonsai/subsystems/logging_manager.hpp"
#include "bonsai/subsystems/engine_event_manager.hpp"
#include "bonsai/subsystems/window_manager.hpp"

namespace bonsai::subsystems
{
    bool MainLoopManager::init()
    {
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
            EngineEventManager::get().run();
            LoggingManager::get().run();
            WindowManager::get().run();
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
