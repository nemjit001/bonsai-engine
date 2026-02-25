#include "bonsai/subsystems/engine_event_manager.hpp"

namespace bonsai::subsystems
{
    bool EngineEventManager::init()
    {
        return SingletonSubsystem::init();
    }

    void EngineEventManager::shutdown()
    {
        m_subsystems.clear();
    }

    EngineEventManager& EngineEventManager::register_subsystem(IEngineSubsystem* subsystem)
    {
        m_subsystems.push_back(subsystem);
        return *this;
    }

    void EngineEventManager::unregister_subsystem(IEngineSubsystem* subsystem)
    {
        m_subsystems.remove(subsystem);
    }

    void EngineEventManager::notify(EngineEvent const& event)
    {
        for (auto const& system : m_subsystems)
        {
            system->process_event(event);
        }
    }
} // namespace bonsai::subsystems
