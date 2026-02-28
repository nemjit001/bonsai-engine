#include "bonsai/subsystems/engine_event_manager.hpp"

#include <algorithm>

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
        if (std::find(std::begin(m_subsystems), std::end(m_subsystems), subsystem) == std::end(m_subsystems)) {
            m_subsystems.push_back(subsystem);
        }
        
        return *this;
    }

    void EngineEventManager::remove_subsystem(IEngineSubsystem* subsystem)
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
