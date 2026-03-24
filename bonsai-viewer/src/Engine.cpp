#include "Engine.hpp"

#include <filesystem>
#include <SDL3/SDL.h>

#include "Logging.hpp"
#include "Renderer/VulkanRenderBackend.hpp"

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

        // TODO(nemjit001): Render a frame using the active render backend
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

    BONSAI_LOG(LogVerbosity::Info, "Initializing RenderBackend");
    BRenderBackendInitInfo RenderBackendInitInfo{};
    RenderBackendInitInfo.WindowTitle = "Bonsai Viewer";
    RenderBackendInitInfo.WindowWidth = 1280;
    RenderBackendInitInfo.WindowHeight = 720;
    RenderBackendInitInfo.NumFramesInFlight = 2; // Use double buffering

    RenderBackend = std::make_unique<BVulkanRenderBackend>();
    RendererResult const RendererInitResult = RenderBackend->Init(RenderBackendInitInfo);
    if (RendererInitResult != RENDERER_RESULT_OK) {
        return ENGINE_RESULT_RENDERER_BAD_INIT;
    }

    BONSAI_LOG(LogVerbosity::Info, "Initialized Bonsai Viewer");
    return ENGINE_RESULT_OK;
}

void BEngine::Shutdown()
{
    BONSAI_LOG(LogVerbosity::Info, "Shutting down Bonsai Viewer");
    BONSAI_LOG(LogVerbosity::Verbose, "Cleaning up RenderBackend");
    RenderBackend->Shutdown();

    BONSAI_LOG(LogVerbosity::Verbose, "Cleaning up SDL3");
    SDL_Quit();

    BONSAI_LOG(LogVerbosity::Info, "Clean shutdown, Goodbye!");
}
