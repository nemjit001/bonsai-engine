#pragma once
#ifndef BONSAI_CORE_SUBSYSTEMS_MAIN_LOOP_MANAGER_HPP
#define BONSAI_CORE_SUBSYSTEMS_MAIN_LOOP_MANAGER_HPP

#include "engine_subsystem.hpp"
#include "engine_event_manager.hpp"
#include "logging_manager.hpp"
#include "window_manager.hpp"

namespace bonsai::subsystems
{
    class MainLoopManager : public IEngineSubsystem
    {
    public:
        MainLoopManager(
            LoggingManager* logging_manager,
            EngineEventManager* event_manager,
            WindowManager* window_manager
        );

        void run() override;
        void process_event(EngineEvent const& event) override;

    private:
        LoggingManager* m_logging_manager = nullptr;
        EngineEventManager* m_event_manager = nullptr;
        WindowManager* m_window_manager = nullptr;
        bool m_running = true;
    };
} // namespace bonsai::subsystems

#endif //BONSAI_CORE_SUBSYSTEMS_MAIN_LOOP_MANAGER_HPP
