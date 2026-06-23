#include "BonsaiEngine.hpp"

#include <SDL3/SDL.h>
#include "Core.hpp"
#include "BonsaiEngineConfig.hpp" // auto-generated config file

BonsaiEngine::BonsaiEngine() = default;
BonsaiEngine::~BonsaiEngine()
{
    PrintDebugString("Shutting down Bonsai Engine...");
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

    // TODO(nemjit001):
    // - [X] Parse CLI args (before window init)
    // - [ ] Parse engine and project config files (before window init)
    // - [ ] Initialize renderer (after window init)
    // - [ ] Load game world (after renderer, load from path in config or cli args?)

    PrintDebugString("Initialized Bonsai Engine!");
    _isRunning = true;
    return true;
}

void BonsaiEngine::Tick()
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
