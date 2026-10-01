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
        _isRunning = true;
        return true;
    }

    void Application::Shutdown()
    {
        _isRunning = false;
        SPDLOG_INFO("Shutdown complete, goodbye!");
    }

    void Application::Tick()
    {
        //
    }

    void Application::Frame()
    {
        //
    }

    AppResult Application::Run()
    {
        // Initialize application
        if (!Init()) {
            return AppResult_BadInit;
        }

        // Enter main loop
        while (_isRunning)
        {
            // Tick application state
            Tick();

            // Render frame
            Frame();
        }

        // Shut down application
        Shutdown();
        return AppResult_Ok;
    }
} // namespace Bonsai
