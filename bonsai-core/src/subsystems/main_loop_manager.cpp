#include "bonsai/subsystems/main_loop_manager.hpp"
namespace bonsai::subsystems
{
    MainLoopManager::MainLoopManager(
        LoggingManager* logging_manager,
        EngineEventManager* event_manager,
        WindowManager* window_manager
    )
        :
        m_logging_manager(logging_manager),
        m_event_manager(event_manager),
        m_window_manager(window_manager)
    {
        //
    }

    void MainLoopManager::run()
    {
        while (m_running)
        {
            m_event_manager->run();
            m_logging_manager->run();
            m_window_manager->run();
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
