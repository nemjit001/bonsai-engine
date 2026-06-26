#include "BonsaiEngine.hpp"

#include <SDL3/SDL.h>
#include "BonsaiEngineConfig.hpp" // auto-generated config file

static DefaultAllocator gDefaultAllocator = DefaultAllocator(); // Default allocator for use by subsystems.
static EventLoopManager gEventLoopManager = EventLoopManager();

BonsaiEngine::~BonsaiEngine()
{
    PrintDebugString("Shutting down Bonsai Engine...");
    gEventLoopManager.Shutdown();
    SDL_Quit();
    PrintDebugString("Goodbye!");
}

bool BonsaiEngine::Init()
{
    // Start engine initialization
    PrintDebugString("Initializing Bonsai Engine (v%s)", BONSAI_ENGINE_VERSION_STRING);

    // Initialize SDL
    PrintDebugString("Initializing SDL");
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        PrintDebugString("Failed to initialize SDL: %s", SDL_GetError());
        return false;
    }

    // Initialize event loop manager
    PrintDebugString("Initializing EventLoopManager");
    gEventLoopManager.Init(&gDefaultAllocator);

    PrintDebugString("Initialized Bonsai Engine!");
    return true;
}

int32 BonsaiEngine::Run([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    // Initialize engine
    if (!Init()) {
        return BonsaiEngineResult_BadInit;
    }

    // Enter main loop
    while (gEventLoopManager.IsRunning())
    {
        gEventLoopManager.PumpMessages();
    }

    return BonsaiEngineResult_Ok;
}
