#pragma once
#ifndef BONSAI_CORE_SUBSYSTEMS_ENGINE_EVENT_MANAGER_HPP
#define BONSAI_CORE_SUBSYSTEMS_ENGINE_EVENT_MANAGER_HPP

#include <list>
#include "engine_subsystem.hpp"

namespace bonsai::subsystems
{
    class EngineEventManager : public SingletonSubsystem<EngineEventManager>
    {
    private:
        friend SingletonSubsystem;
    public:
        bool init() override;
        void shutdown() override;

        void register_subsystem(IEngineSubsystem* subsystem);

        void unregister_subsystem(IEngineSubsystem* subsystem);

        void notify(EngineEvent const& event);

    private:
        std::list<IEngineSubsystem*> m_subsystems;
    };
} // namespace bonsai::subsystems

#endif //BONSAI_CORE_SUBSYSTEMS_ENGINE_EVENT_MANAGER_HPP
