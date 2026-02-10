#include <bonsai/subsystems/logging_manager.hpp>
#include <bonsai/subsystems/engine_event_manager.hpp>
#include <bonsai/subsystems/window_manager.hpp>
namespace bonsai::subsystems
{
    class MainLoopManager : public SingletonSubsystem<MainLoopManager>
    {
    private:
        friend SingletonSubsystem;
    public:
        bool init() override
        {
            m_running = true;
            return true;
        }

        void shutdown() override
        {
            m_running = false;
        }

        void run() override
        {
            while (m_running)
            {
                LoggingManager::get().run();
                EngineEventManager::get().run();
                WindowManager::get().run();
            }
        }

        void process_event(EngineEvent const& event) override
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

    private:
        bool m_running = false;
    };
} // namespace bonsai::subsystems

int main(int, char*[])
{
    auto& logging_manager = bonsai::subsystems::LoggingManager::get();
    if (!logging_manager.init()) {
        return 1;
    }

    auto& engine_event_manager = bonsai::subsystems::EngineEventManager::get();
    if (!engine_event_manager.init()) {
        return 1;
    }

    auto& window_manager = bonsai::subsystems::WindowManager::get();
    if (!window_manager.init()) {
        return 1;
    }

    auto& main_loop_manager = bonsai::subsystems::MainLoopManager::get();
    if (!main_loop_manager.init()) {
        return 1;
    }

    engine_event_manager.register_subsystem(&logging_manager);
    engine_event_manager.register_subsystem(&window_manager);
    engine_event_manager.register_subsystem(&main_loop_manager);

    main_loop_manager.run();

    main_loop_manager.shutdown();
    window_manager.shutdown();
    engine_event_manager.shutdown();
    logging_manager.shutdown();
    return 0;
}
