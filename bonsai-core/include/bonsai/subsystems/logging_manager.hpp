#pragma once
#ifndef BONSAI_CORE_SUBSYSTEMS_LOGGING_MANAGER_HPP
#define BONSAI_CORE_SUBSYSTEMS_LOGGING_MANAGER_HPP

#include "engine_subsystem.hpp"

namespace bonsai::subsystems
{
    /// @brief The logging subsystem manages internal logging structures.
    class LoggingManager : public SingletonSubsystem<LoggingManager>
    {
    private:
        friend IEngineSubsystem;
    public:
        bool init() override;
        void shutdown() override;
    };
} // namespace bonsai::subsystems

#endif //BONSAI_CORE_SUBSYSTEMS_LOGGING_MANAGER_HPP
