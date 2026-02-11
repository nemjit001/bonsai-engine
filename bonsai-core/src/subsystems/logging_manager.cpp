#include "bonsai/subsystems/logging_manager.hpp"

namespace bonsai::subsystems
{
    bool LoggingManager::init()
    {
        spdlog::set_level(spdlog::level::trace);
        return true;
    }

    void LoggingManager::shutdown()
    {
        //
    }
} // namespace bonsai::subsystems
