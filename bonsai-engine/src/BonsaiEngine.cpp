#include "BonsaiEngine.hpp"

#include <SDL3/SDL.h>
#include "BonsaiEngineConfig.hpp" // auto-generated config file

/// @brief Number of ticks per second to process.
static constexpr float BONSAI_TICKS_PER_SECOND = 120.0F;
/// @brief Time delta between ticks in milliseconds.
static constexpr float BONSAI_TICK_TIMESTEP_MILLIS = 1'000.0F / BONSAI_TICKS_PER_SECOND;
/// @brief Time delta between ticks in seconds.
static constexpr float BONSAI_TICK_TIMESTEP_SECONDS = BONSAI_TICK_TIMESTEP_MILLIS * 1e-3;

BonsaiEngine::BonsaiEngine() = default;
BonsaiEngine::~BonsaiEngine()
{
    PrintDebugString("Shutting down Bonsai Engine...");
    _world.reset();
    _scheduler.reset();
    SDL_Quit();
    
    PrintDebugString("Goodbye!");
}

bool BonsaiEngine::Init(int argc, char* argv[])
{
    // Start engine initialization
    PrintDebugString("Initializing Bonsai Engine (v%s)", BONSAI_ENGINE_VERSION_STRING);

    // Set up default allocator
    DefaultAllocator defaultAllocator{};

    // Initialize CLI arg parser
    CLIArgParser cliArgs(&defaultAllocator);
    cliArgs.AddFlag('h', "help");

    if (!cliArgs.Parse(argc, argv))
    {
        PrintDebugString("Failed to parse CLI arguments");
        return false;
    }

    if (cliArgs.IsSet('h'))
    {
        // TODO(nemjit001): Dump help to logfile / primary console output
        PrintDebugString("Displaying Bonsai Engine help dialog");
        PrintDebugString("Usage:\tBonsaiEngine [options]");
        PrintDebugString("");
        PrintDebugString("Flags:");
        PrintDebugString("\t-h, --help\tShow this help dialog");
        return true;
    }

    // Initialize SDL
    PrintDebugString("Initializing SDL");
    SDL_SetMemoryFunctions(Memory::Malloc, Memory::Calloc, Memory::Realloc, Memory::Free);
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        PrintDebugString("Failed to initialize SDL");
        return false;
    }

    // Initialize system scheduler
    PrintDebugString("Initializing SystemScheduler");
    _scheduler = std::make_unique<SystemScheduler>(&defaultAllocator);

    // Initialize game world
    PrintDebugString("Initializing World");
    _world = std::make_unique<World>();
    // TODO(nemjit001): Load default level asset (read from config or cli args override)

    // TODO(nemjit001):
    // - [X] Parse CLI args (before window init)
    // - [ ] Parse engine and project config files (before window init)
    // - [ ] Initialize main application window
    // - [ ] Initialize renderer (after window init)
    // - [X] Initialize ECS system scheduler
    // - [ ] Initialize default systems (rendering, asset loading, etc.)
    // - [ ] Initialize application (registers app systems)
    // - [X] Initialize game world (after renderer and system scheduler, load from path in config or cli args?)

    PrintDebugString("Initialized Bonsai Engine!");
    _isRunning = true;
    return true;
}

void BonsaiEngine::Tick()
{
    // Update frame time
    static double prevTime = Timer::GetTime();
    static double currTime = Timer::GetTime();
    static double frametimeAccumulator = 0.0; // Accumulator for unprocessed tick time.
    prevTime = currTime;
    currTime = Timer::GetTime();

    // Calculate frame time delta
    double const deltaTime = std::clamp(currTime - prevTime, 0.0, 250.0); // above 250 ms we asume a hang occured and continue as normal.
    frametimeAccumulator += deltaTime;

    // Tick systems on fixed time step
    BONSAI_ASSERT(_scheduler && "SystemScheduler is nullptr!");
    BONSAI_ASSERT(_world && "World is nullptr!");
    while (frametimeAccumulator > BONSAI_TICK_TIMESTEP_MILLIS)
    {
        // Handle SDL platform events
        {
            SDL_Event event{};
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_EVENT_QUIT) {
                    _isRunning = false;
                }
            }
        }

        // Tick ECS systems
        _scheduler->RunSystems(*_world, BONSAI_TICK_TIMESTEP_SECONDS); // uses timestep in seconds
        frametimeAccumulator -= BONSAI_TICK_TIMESTEP_MILLIS;
    }

    // TODO(nemjit001): render current world state
}

int32 BonsaiEngine::Run(int argc, char* argv[])
{
    if (!Init(argc, argv)) {
        return BonsaiEngineResult_BadInit;
    }

    while (_isRunning) {
        Tick();
    }

    return BonsaiEngineResult_Ok;
}
