#include "Application.hpp"

#include "Logger.hpp"

namespace Bonsai
{
    bool Application::Init()
    {
        // Initialize default logger
        spdlog::set_level(Bonsai::Logger::DEFAULT_LOG_LEVEL);
        SPDLOG_TRACE("Initialized Logger");

        // Initialized!
        SPDLOG_INFO("Initialized Bonsai!");
        return true;
    }

    void Application::Shutdown()
    {
        SPDLOG_INFO("Shutdown complete, goodbye!");
    }

    AppResult Application::Run()
    {
        if (!Init()) {
            return AppResult_BadInit;
        }

        Shutdown();
        return AppResult_Ok;
    }
} // namespace Bonsai
