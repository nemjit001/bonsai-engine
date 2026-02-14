#include <bonsai/subsystems/engine_event_manager.hpp>
#include <bonsai/subsystems/logging_manager.hpp>
#include <bonsai/subsystems/main_loop_manager.hpp>
#include <bonsai/subsystems/render_manager.hpp>
#include <bonsai/subsystems/scene_manager.hpp>
#include <bonsai/subsystems/window_manager.hpp>

int main(int, char*[])
{
    // Initialize engine systems
    auto& logging_manager = bonsai::subsystems::LoggingManager::get();
    if (!logging_manager.init()) {
        return 1;
    }
    SPDLOG_TRACE("Initialized Logging Manager");

    SPDLOG_TRACE("Initializing Engine Event Manager");
    auto& engine_event_manager = bonsai::subsystems::EngineEventManager::get();
    if (!engine_event_manager.init()) {
        return 1;
    }

    SPDLOG_TRACE("Initializing Window Manager");
    auto window_manager = bonsai::subsystems::WindowManager();
    if (!window_manager.init()) {
        return 1;
    }

    SPDLOG_TRACE("Initializing Scene Manager");
    auto scene_manager = bonsai::subsystems::SceneManager();
    if (!scene_manager.init({})) {
        return 1;
    }

    SPDLOG_TRACE("Initializing Render Manager");
    auto render_manager = bonsai::subsystems::RenderManager(&window_manager);
    if (!render_manager.init()) {
        return 1;
    }

    SPDLOG_TRACE("Initializing Main Loop Manager");
    auto main_loop_manager = bonsai::subsystems::MainLoopManager(
        &logging_manager,
        &engine_event_manager,
        &window_manager,
        &scene_manager,
        &render_manager
    );
    if (!main_loop_manager.init()) {
        return 1;
    }

    engine_event_manager.register_subsystem(&logging_manager);
    engine_event_manager.register_subsystem(&window_manager);
    engine_event_manager.register_subsystem(&scene_manager);
    engine_event_manager.register_subsystem(&render_manager);
    engine_event_manager.register_subsystem(&main_loop_manager);
    SPDLOG_INFO("Initialized Bonsai");

    // Run main loop
    main_loop_manager.run();

    // Shut down engine systems
    SPDLOG_INFO("Shutting down Bonsai");
    SPDLOG_TRACE("Shutting down Main Loop Manager");
    main_loop_manager.shutdown();
    SPDLOG_TRACE("Shutting down Render Manager");
    render_manager.shutdown();
    SPDLOG_TRACE("Shutting down Window Manager");
    window_manager.shutdown();
    SPDLOG_TRACE("Shutting down Engine Event Manager");
    engine_event_manager.shutdown();
    SPDLOG_TRACE("Shutting Logging Manager");
    logging_manager.shutdown();
    return 0;
}
