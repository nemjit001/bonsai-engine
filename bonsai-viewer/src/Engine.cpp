#include "Engine.hpp"

#include <SDL3/SDL.h>

#include "Renderer.hpp"

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

        // Render frame
        Renderer->Frame();
    }

    // Perform cleanup
    Shutdown();
    return ENGINE_RESULT_OK;
}

EngineResult BEngine::Init()
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return ENGINE_RESULT_PLATFORM_BAD_INIT;
    }

    Renderer = std::make_unique<BRenderer>();
    RendererResult const RendererInitResult = Renderer->Init();
    if (RendererInitResult != RENDERER_RESULT_OK) {
        return ENGINE_RESULT_RENDERER_BAD_INIT;
    }

    return ENGINE_RESULT_OK;
}

void BEngine::Shutdown()
{
    Renderer->Shutdown();
    SDL_Quit();
}
