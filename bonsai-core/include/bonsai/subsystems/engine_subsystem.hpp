#pragma once
#ifndef BONSAI_CORE_SUBSYSTEMS_ENGINE_SUBSYSTEM_HPP
#define BONSAI_CORE_SUBSYSTEMS_ENGINE_SUBSYSTEM_HPP

#include "bonsai/engine_event.hpp"

namespace bonsai::subsystems
{
    /// @brief Engine subsystem interface. Engine systems should implement this to send and receive subsystem events.
    class IEngineSubsystem
    {
    public:
        virtual ~IEngineSubsystem() = default;

        /// @brief Initialize the engine subsystem.
        virtual void init() = 0;

        /// @brief Shut down the engine subsystem.
        virtual void shutdown() = 0;

        /// @brief Run the engine subsystem a single time.
        virtual void run() = 0;

        /// @brief Process an engine event.
        /// @param event Event to process.
        virtual void process_event(EngineEvent const& event) = 0;

        /// @brief Notify other subsystems of an engine event.
        /// @param event Event to notify other systems of.
        virtual void notify(EngineEvent const& event) = 0;
    };
} // namespace bonsai::subsystems

#endif //BONSAI_CORE_SUBSYSTEMS_ENGINE_SUBSYSTEM_HPP
