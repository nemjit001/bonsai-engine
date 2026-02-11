#include "bonsai/subsystems/engine_subsystem.hpp"

namespace bonsai::subsystems
{
    bool IEngineSubsystem::init()
    {
        return true;
    }

    void IEngineSubsystem::shutdown()
    {
        //
    }

    void IEngineSubsystem::run()
    {
        //
    }

    void IEngineSubsystem::process_event([[maybe_unused]] EngineEvent const& event)
    {
        //
    }
} // namespace bonsai::subsystems
