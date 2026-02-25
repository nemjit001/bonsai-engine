#pragma once
#ifndef BONSAI_CORE_SUBSYSTEMS_ENGINE_EVENT_MANAGER_HPP
#define BONSAI_CORE_SUBSYSTEMS_ENGINE_EVENT_MANAGER_HPP

#include <list>
#include "engine_subsystem.hpp"

namespace bonsai::subsystems
{
    /// @brief The engine event subsystem manages notifying other subsystems of engine events.
    class EngineEventManager : public SingletonSubsystem<EngineEventManager>
    {
    private:
        friend SingletonSubsystem;
        EngineEventManager() = default;
    public:
        [[nodiscard]] bool init() override;
        void shutdown() override;

        /// @brief Register a subsystem with the event manager using a builder pattern.
        /// @param subsystem 
        /// @return 
        EngineEventManager& register_subsystem(IEngineSubsystem* subsystem);

        /// @brief Deregister a subsystem from the event manager.
        /// @param subsystem 
        void unregister_subsystem(IEngineSubsystem* subsystem);

        /// @brief Notify all registered subsystems of an engine event.
        /// @param event Engine event to publish.
        void notify(EngineEvent const& event);

    private:
        std::list<IEngineSubsystem*> m_subsystems;
    };
} // namespace bonsai::subsystems

#endif //BONSAI_CORE_SUBSYSTEMS_ENGINE_EVENT_MANAGER_HPP
