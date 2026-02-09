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
        IEngineSubsystem() = default;
        virtual ~IEngineSubsystem() = default;

        IEngineSubsystem(IEngineSubsystem const&) = delete;
        IEngineSubsystem& operator=(IEngineSubsystem const&) = delete;

        /// @brief Initialize the engine subsystem.
        /// @return A boolean indicating successful initialization.
        [[nodiscard]]
        virtual bool init() = 0;

        /// @brief Shut down the engine subsystem.
        virtual void shutdown() = 0;

        /// @brief Run the engine subsystem a single time.
        virtual void run();

        /// @brief Process an engine event.
        /// @param event Event to process.
        virtual void process_event(EngineEvent const& event);

        /// @brief Notify other subsystems of an engine event.
        /// @param event Event to notify other systems of.
        void notify(EngineEvent const& event);
    };
} // namespace bonsai::subsystems

#endif //BONSAI_CORE_SUBSYSTEMS_ENGINE_SUBSYSTEM_HPP
