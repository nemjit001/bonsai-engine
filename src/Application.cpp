#include "Application.hpp"

#include <SDL3/SDL.h>
#include "Core/Logger.hpp"

namespace Bonsai
{
    bool Application::Init()
    {
        // Initialize default logger
        spdlog::set_level(Bonsai::Logger::DEFAULT_LOG_LEVEL);
        SPDLOG_TRACE("Initialized Logger");

        // Initialize SDL
        SPDLOG_TRACE("Initializing SDL");
        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            SPDLOG_ERROR("Failed to initialize SDL");
            return false;
        }

        // Initialized!
        SPDLOG_INFO("Initialized Bonsai!");
        _isRunning = true;
        return true;
    }

    void Application::Shutdown()
    {
        _isRunning = false;
        SDL_Quit();
        SPDLOG_INFO("Shutdown complete, goodbye!");
    }

    void Application::PumpMessages()
    {
        SDL_Event event{};
        while (SDL_PollEvent(&event))
        {
            HandlePlatformEvent(event);
        }
    }

    void Application::HandlePlatformEvent(SDL_Event const& event)
    {
        // Handle quit event
        if (event.type == SDL_EVENT_QUIT) {
            _isRunning = false;
        }
    }

    void Application::Tick()
    {
        // Pump platform message loop.
        PumpMessages();

        // Tick application systems
        _sceneManager.Tick();
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
