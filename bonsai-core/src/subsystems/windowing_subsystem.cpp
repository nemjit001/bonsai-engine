#include "bonsai/subsystems/windowing_subsystem.hpp"

#include <SDL3/SDL.h>

namespace bonsai::subsystems
{
    bool WindowingSystem::init()
    {
        return SDL_Init(SDL_INIT_VIDEO);
    }

    void WindowingSystem::shutdown()
    {
        SDL_Quit();
    }

    void WindowingSystem::run()
    {
        SDL_Event event{};
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                notify(EngineEvent{ BONSAI_ENGINE_EVENT_TYPE_QUIT });
                break;
            default:
                break;
            }
        }
    }

    void WindowingSystem::process_event([[maybe_unused]] EngineEvent const& event)
    {
        //
    }
} // namespace bonsai::subsystems
