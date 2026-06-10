#include "BonsaiEngine.hpp"

#include <SDL3/SDL.h>
#include "BonsaiEngineConfig.hpp" // auto-generated config file

static constexpr char const* BONSAI_DEFAULT_WINDOW_TITLE    = "Bonsai Engine";
static constexpr uint32 BONSAI_DEFAULT_WINDOW_WIDTH         = 1280u;
static constexpr uint32 BONSAI_DEFAULT_WINDOW_HEIGHT        = 720u;

static SDL_Window* gMainWindow = nullptr;

BonsaiEngine::BonsaiEngine() = default;
BonsaiEngine::~BonsaiEngine()
{
    PrintDebugString("Shutting down Bonsai Engine...");
    SDL_DestroyWindow(gMainWindow);
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
        PrintDebugString("Help requested!");
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

    // Create a default application window
    PrintDebugString("Creating application window");
    gMainWindow = SDL_CreateWindow(
        BONSAI_DEFAULT_WINDOW_TITLE,
        BONSAI_DEFAULT_WINDOW_WIDTH,
        BONSAI_DEFAULT_WINDOW_HEIGHT,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY
    );
    if (gMainWindow == nullptr)
    {
        PrintDebugString("Failed to create application window");
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

            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED
                && event.window.windowID == SDL_GetWindowID(gMainWindow)) {
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
