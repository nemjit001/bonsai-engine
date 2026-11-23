#include "Application.hpp"

#include <SDL3/SDL.h>
#include "Core/Logger.hpp"
#include "Systems/CameraInitSystem.hpp"
#include "Systems/RenderWorldSyncSystem.hpp"
#include "Application/FileDropHandler.hpp"

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

        // Initialize render manager
        SPDLOG_TRACE("Initializing RenderManager");
        if (!_renderManager.Initialize())
        {
            SPDLOG_ERROR("Failed to initialize RenderManager");
            return false;
        }

        // Initialize the scene manager
        SPDLOG_TRACE("Initializing SceneManager");
        if (!_sceneManager.Initialize())
        {
            SPDLOG_ERROR("Failed to initialize SceneManager");
            return false;
        }

        // Register ECS systems
        _systemRegistry.AddSystem<CameraInitSystem>();
        _systemRegistry.AddSystem<RenderWorldSyncSystem>(&_renderManager);

        // Initialized!
        SPDLOG_INFO("Initialized Bonsai!");
        _isRunning = true;
        return true;
    }

    void Application::Shutdown()
    {
        _isRunning = false;
        _systemRegistry.Clear();
        _sceneManager.Shutdown();
        _renderManager.Shutdown();
        SDL_Quit();
        SPDLOG_INFO("Shutdown complete, goodbye!");
    }

    void Application::PumpMessages()
    {
        SDL_Event event{};
        while (SDL_PollEvent(&event))
        {
            HandlePlatformEvent(event);
            _renderManager.HandlePlatformEvent(event);
        }
    }

    void Application::HandlePlatformEvent(SDL_Event const& event)
    {
        // Handle quit event
        if (event.type == SDL_EVENT_QUIT) {
            _isRunning = false;
        }
        else if (event.type == SDL_EVENT_DROP_FILE)
        {
            Filesystem::Path const droppedFile{event.drop.data};
            SPDLOG_TRACE("Received dropped file: {}", droppedFile.string());
            FileDropHandler::HandleDroppedFile(droppedFile, _assetManager, _sceneManager);
        }
    }

    void Application::Tick()
    {
        // Pump platform message loop
        PumpMessages();
        
        // Start a new frame
        if (!_renderManager.StartFrame()) {
            return;
        }

        // Tick the scene using the system registry
        Scene& scene = _sceneManager.GetActiveScene();
        scene.Tick(_systemRegistry);

        // Render the current frame
        _renderManager.RenderFrame();
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
        }

        // Shut down application
        Shutdown();
        return AppResult_Ok;
    }
} // namespace Bonsai
