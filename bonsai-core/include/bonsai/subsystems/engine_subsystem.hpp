#pragma once
#ifndef BONSAI_CORE_SUBSYSTEMS_ENGINE_SUBSYSTEM_HPP
#define BONSAI_CORE_SUBSYSTEMS_ENGINE_SUBSYSTEM_HPP

#include "bonsai/subsystems/engine_event.hpp"

namespace bonsai::subsystems
{
    /// @brief Engine subsystem interface. Engine systems should implement this to implement subsystem behaviour and receive subsystem events.
    class IEngineSubsystem
    {
    public:
        IEngineSubsystem() = default;
        virtual ~IEngineSubsystem() = default;

        IEngineSubsystem(IEngineSubsystem const&) = default;
        IEngineSubsystem& operator=(IEngineSubsystem const&) = default;

        /// @brief Run the engine subsystem a single time.
        virtual void run();

        /// @brief Process an engine event.
        /// @param event Event to process.
        virtual void process_event(EngineEvent const& event);
    };

    /// @brief Singleton subsystem class. Subsystems should derive from this class if they need to be globally accessible (e.g. logging systems).
    /// The singleton should be initialized and shut down through the respective functions to avoid unexpectedly expensive Singleton::get() calls.
    /// @tparam SubsystemType Subsystem derived type.
    template<class SubsystemType>
    class SingletonSubsystem : public IEngineSubsystem
    {
    protected:
        SingletonSubsystem() = default;
    public:
        ~SingletonSubsystem() = default;

        SingletonSubsystem(SingletonSubsystem const&) = delete;
        SingletonSubsystem& operator=(SingletonSubsystem const&) = delete;

        /// @brief Get the static class instance.
        /// @return 
        static SubsystemType& get();

        /// @brief Initialize the engine subsystem.
        /// @return A boolean indicating successful initialization.
        virtual bool init() = 0;

        /// @brief Shut down the engine subsystem.
        virtual void shutdown() = 0;
    };

    template<class SubsystemType>
    SubsystemType& SingletonSubsystem<SubsystemType>::get()
    {
        static SubsystemType instance{};
        return instance;
    }
} // namespace bonsai::subsystems

#endif //BONSAI_CORE_SUBSYSTEMS_ENGINE_SUBSYSTEM_HPP
