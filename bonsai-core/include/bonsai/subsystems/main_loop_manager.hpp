#pragma once
#ifndef BONSAI_CORE_SUBSYSTEMS_MAIN_LOOP_MANAGER_HPP
#define BONSAI_CORE_SUBSYSTEMS_MAIN_LOOP_MANAGER_HPP

#include "engine_subsystem.hpp"

namespace bonsai::subsystems
{
    class MainLoopManager : public SingletonSubsystem<MainLoopManager>
    {
    private:
        friend SingletonSubsystem;
        MainLoopManager() = default;
    public:
        bool init() override;
        void shutdown() override;
        void run() override;
        void process_event(EngineEvent const& event) override;

    private:
        bool m_running = false;
    };
} // namespace bonsai::subsystems

#endif //BONSAI_CORE_SUBSYSTEMS_MAIN_LOOP_MANAGER_HPP
