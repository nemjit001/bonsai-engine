#pragma once
#ifndef BONSAI_CORE_SUBSYSTEMS_WINDOWING_SYSTEM_HPP
#define BONSAI_CORE_SUBSYSTEMS_WINDOWING_SYSTEM_HPP

#include "engine_subsystem.hpp"

namespace bonsai::subsystems
{
    /// @brief The windowing subsystem manages platform windows and event processing.
    class WindowingSystem : public IEngineSubsystem
    {
    public:
        bool init() override;

        void shutdown() override;

        void run() override;

        void process_event(EngineEvent const& event) override;
    };
} // namespace bonsai::subsystems

#endif //BONSAI_CORE_SUBSYSTEMS_WINDOWING_SYSTEM_HPP
