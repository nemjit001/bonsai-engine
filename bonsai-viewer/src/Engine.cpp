#include "Engine.hpp"

#include <filesystem>
#include <SDL3/SDL.h>

#include "Logging.hpp"
#include "Renderer/VulkanRenderer.hpp"

BEngine::BEngine() = default;
BEngine::~BEngine() = default;

EngineResult BEngine::Run()
{
    // Initialize engine
    EngineResult const InitResult = Init();
    if (InitResult != ENGINE_RESULT_OK) {
        return InitResult;
    }
    
    // Run main loop
    bool Running = true;
    while (Running)
    {
        // Pump platform event queue
        SDL_Event Event{};
        while (SDL_PollEvent(&Event))
        {
            if (Event.type == SDL_EVENT_QUIT) {
                Running = false;
            }
        }

        // TODO(nemjit001): Render frame
    }

    // Perform cleanup
    Shutdown();
    return ENGINE_RESULT_OK;
}

EngineResult BEngine::Init()
{
    std::filesystem::path CurrentPath = std::filesystem::current_path();
    BONSAI_LOG(LogVerbosity::Info, "Initializing Bonsai Viewer from %s", CurrentPath.string().c_str());

    BONSAI_LOG(LogVerbosity::Info, "Initializing SDL3");
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return ENGINE_RESULT_PLATFORM_BAD_INIT;
    }

    BONSAI_LOG(LogVerbosity::Info, "Initializing Renderer");
    Renderer = std::make_unique<BVulkanRenderer>();
    RendererResult const RendererInitResult = Renderer->Init();
    if (RendererInitResult != RENDERER_RESULT_OK) {
        return ENGINE_RESULT_RENDERER_BAD_INIT;
    }

    BONSAI_LOG(LogVerbosity::Info, "Initialized Bonsai Viewer");
    return ENGINE_RESULT_OK;
}

void BEngine::Shutdown()
{
    BONSAI_LOG(LogVerbosity::Info, "Shutting down Bonsai Viewer");
    BONSAI_LOG(LogVerbosity::Verbose, "Cleaning up Renderer");
    Renderer->Shutdown();

    BONSAI_LOG(LogVerbosity::Verbose, "Cleaning up SDL3");
    SDL_Quit();

    BONSAI_LOG(LogVerbosity::Info, "Clean shutdown, Goodbye!");
}
