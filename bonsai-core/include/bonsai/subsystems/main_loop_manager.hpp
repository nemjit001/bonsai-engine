#pragma once
#ifndef BONSAI_CORE_SUBSYSTEMS_MAIN_LOOP_MANAGER_HPP
#define BONSAI_CORE_SUBSYSTEMS_MAIN_LOOP_MANAGER_HPP

#include "engine_subsystem.hpp"

namespace bonsai::subsystems
{
    class ECSManager;
    class EngineEventManager;
    class LoggingManager;
    class RenderManager;
    class SceneManager;
    class WindowManager;

    /// @brief The main loop manager handles updating other engine subsystems in order.
    class MainLoopManager : public IEngineSubsystem
    {
    public:
        MainLoopManager(
            LoggingManager* logging_manager,
            EngineEventManager* event_manager,
            WindowManager* window_manager,
            SceneManager* scene_manager,
            ECSManager* ecs_manager,
            RenderManager* render_manager
        );

        [[nodiscard]] bool init() override;
        void shutdown() override;
        void run() override;
        void process_event(EngineEvent const& event) override;

    private:
        LoggingManager* m_logging_manager = nullptr;
        EngineEventManager* m_event_manager = nullptr;
        WindowManager* m_window_manager = nullptr;
        SceneManager* m_scene_manager = nullptr;
        ECSManager* m_ecs_manager = nullptr;
        RenderManager* m_render_manager = nullptr;
        bool m_running = false;
    };
} // namespace bonsai::subsystems

#endif //BONSAI_CORE_SUBSYSTEMS_MAIN_LOOP_MANAGER_HPP
