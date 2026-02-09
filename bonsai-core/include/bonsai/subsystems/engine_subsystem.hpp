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

    /// @brief The engine subsystem registry contains other engine subsystems and provides a static interface for subsystem management.
    class EngineSubsystemRegistry : public IEngineSubsystem
    {
    public:
        bool init() override;

        void shutdown() override;

        /// @brief Register an engine subsystem.
        /// @param subsystem 
        static void register_subsystem(IEngineSubsystem* subsystem);

        /// @brief Unregister an engine subsystem.
        /// @param subsystem 
        static void unregister_subsystem(IEngineSubsystem* subsystem);

        /// @brief Notify all registered engine subsystems of an engine event.
        /// @param event 
        static void notify_subsystems(EngineEvent const& event);

    private:
        struct Impl;
        static Impl* s_pImpl;
    };
} // namespace bonsai::subsystems

#endif //BONSAI_CORE_SUBSYSTEMS_ENGINE_SUBSYSTEM_HPP
