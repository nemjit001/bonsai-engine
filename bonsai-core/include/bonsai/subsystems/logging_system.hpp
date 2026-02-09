#pragma once
#ifndef BONSAI_CORE_SUBSYSTEMS_LOGGING_SYSTEM_HPP
#define BONSAI_CORE_SUBSYSTEMS_LOGGING_SYSTEM_HPP

#include "engine_subsystem.hpp"

namespace bonsai::subsystems
{
    /// @brief The logging subsystem manages internal logging structures.
    class LoggingSystem : public IEngineSubsystem
    {
    public:
        bool init() override;

        void shutdown() override;
    };
} // namespace bonsai::subsystems

#endif //BONSAI_CORE_SUBSYSTEMS_LOGGING_SYSTEM_HPP
