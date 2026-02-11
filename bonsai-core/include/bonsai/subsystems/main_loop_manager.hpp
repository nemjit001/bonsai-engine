#pragma once
#ifndef BONSAI_CORE_SUBSYSTEMS_MAIN_LOOP_MANAGER_HPP
#define BONSAI_CORE_SUBSYSTEMS_MAIN_LOOP_MANAGER_HPP

#include "engine_subsystem.hpp"

namespace bonsai::subsystems
{
    class LoggingManager;
    class EngineEventManager;
    class RenderManager;
    class WindowManager;

    /// @brief The main loop manager handles updating other engine subsystems in order.
    class MainLoopManager : public IEngineSubsystem
    {
    public:
        MainLoopManager(
            LoggingManager* logging_manager,
            EngineEventManager* event_manager,
            WindowManager* window_manager,
            RenderManager* render_manager
        );

        bool init() override;
        void shutdown() override;
        void run() override;
        void process_event(EngineEvent const& event) override;

    private:
        LoggingManager* m_logging_manager = nullptr;
        EngineEventManager* m_event_manager = nullptr;
        WindowManager* m_window_manager = nullptr;
        RenderManager* m_render_manager = nullptr;
        bool m_running = false;
    };
} // namespace bonsai::subsystems

#endif //BONSAI_CORE_SUBSYSTEMS_MAIN_LOOP_MANAGER_HPP
