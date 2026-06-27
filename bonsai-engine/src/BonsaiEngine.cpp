#include "BonsaiEngine.hpp"

#include <SDL3/SDL.h>
#include "BonsaiEngineConfig.hpp" // auto-generated config file

static DefaultAllocator gDefaultAllocator   = DefaultAllocator(); // Default allocator for use by subsystems.
static EventLoopManager gEventLoopManager   = EventLoopManager();
static RenderManager    gRenderManager      = RenderManager();

BonsaiEngine::~BonsaiEngine()
{
    PrintDebugString("Shutting down Bonsai Engine...");
    gRenderManager.Shutdown();
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
    SDL_SetMemoryFunctions(Memory::Malloc, Memory::Calloc, Memory::Realloc, Memory::Free);
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        PrintDebugString("Failed to initialize SDL: %s", SDL_GetError());
        return false;
    }

    // Initialize event loop manager
    PrintDebugString("Initializing EventLoopManager");
    if (!gEventLoopManager.Init(&gDefaultAllocator))
    {
        PrintDebugString("Failed to initialize EventLoopManager");
        return false;
    }

    // Initialize render manager
    PrintDebugString("Initializing RenderManager");
    RenderManagerInitInfo rmInitInfo{};
    rmInitInfo.windowTitle = "Bonsai Engine"sv;
    rmInitInfo.windowWidth = 1280;
    rmInitInfo.windowHeight = 720;

    gEventLoopManager.RegisterSubsystem(&gRenderManager);
    if (!gRenderManager.Init(rmInitInfo))
    {
        PrintDebugString("Failed to initialize RenderManager");
        return false;
    }

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
    double prevTime = Timer::GetTime();
    double currTime = prevTime;
    while (gEventLoopManager.IsRunning())
    {
        // Tick frame timer
        prevTime = currTime;
        currTime = Timer::GetTime();

        // Ensure frame time is always positive, above 100ms we assume a frame hang and just continue with this as time delta
        [[maybe_unused]]
        double const deltaTimeMillis = std::clamp(currTime - prevTime, 0.0, 100.0);

        // Update engine subsystems
        gRenderManager.NewFrame();
        gEventLoopManager.PumpMessages();

        // TODO(nemjit001): Tick world state (fixed timestep) and render (using interpolation of leftover frame time)
        gRenderManager.EndFrame();
    }

    return BonsaiEngineResult_Ok;
}
