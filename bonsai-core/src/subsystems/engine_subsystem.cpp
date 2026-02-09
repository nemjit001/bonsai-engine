#include "bonsai/subsystems/engine_subsystem.hpp"

#include <algorithm>
#include <list>

namespace bonsai::subsystems
{
    /// @brief Engine subsystem registry implementation.
    struct EngineSubsystemRegistry::Impl
    {
        std::list<IEngineSubsystem*> subsystems;
    };
    EngineSubsystemRegistry::Impl* EngineSubsystemRegistry::s_pImpl = nullptr;

    void IEngineSubsystem::run()
    {
        //
    }

    void IEngineSubsystem::notify(EngineEvent const& event)
    {
        EngineSubsystemRegistry::notify_subsystems(event);
    }

    void IEngineSubsystem::process_event([[maybe_unused]] EngineEvent const& event)
    {
        //
    }

    bool EngineSubsystemRegistry::init()
    {
        if (s_pImpl == nullptr)
        {
            s_pImpl = new Impl{};
        }
        return true;
    }

    void EngineSubsystemRegistry::shutdown()
    {
        while (s_pImpl != nullptr && !s_pImpl->subsystems.empty())
        {
            unregister_subsystem(s_pImpl->subsystems.back());
        }

        delete s_pImpl;
        s_pImpl = nullptr;
    }

    void EngineSubsystemRegistry::register_subsystem(IEngineSubsystem* subsystem)
    {
        if (s_pImpl == nullptr)
        {
            return;
        }

        s_pImpl->subsystems.push_back(subsystem);
    }

    void EngineSubsystemRegistry::unregister_subsystem(IEngineSubsystem* subsystem)
    {
        if (s_pImpl == nullptr)
        {
            return;
        }

        s_pImpl->subsystems.remove(subsystem);
    }

    void EngineSubsystemRegistry::notify_subsystems(EngineEvent const& event)
    {
        if (s_pImpl == nullptr)
        {
            return;
        }

        for (auto const& subsystem : s_pImpl->subsystems)
        {
            subsystem->notify(event);
        }
    }
} // namespace bonsai::subsystems
