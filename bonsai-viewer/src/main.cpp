#include <bonsai/subsystems/engine_event_manager.hpp>
#include <bonsai/subsystems/logging_manager.hpp>
#include <bonsai/subsystems/main_loop_manager.hpp>
#include <bonsai/subsystems/window_manager.hpp>

int main(int, char*[])
{
    // Initialize engine systems
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

    // Run main loop
    main_loop_manager.run();

    // Shut down engine systems
    main_loop_manager.shutdown();
    window_manager.shutdown();
    engine_event_manager.shutdown();
    logging_manager.shutdown();
    return 0;
}
