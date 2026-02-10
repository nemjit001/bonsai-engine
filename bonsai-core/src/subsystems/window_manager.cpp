#include "bonsai/subsystems/window_manager.hpp"

#include <SDL3/SDL.h>
#include "bonsai/subsystems/engine_event_manager.hpp"

namespace bonsai::subsystems
{
    bool WindowManager::init()
    {
        return SDL_Init(SDL_INIT_VIDEO);
    }

    void WindowManager::shutdown()
    {
        SDL_Quit();
    }

    void WindowManager::run()
    {
        auto& event_manager = EngineEventManager::get();

        SDL_Event event{};
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                event_manager.notify(EngineEvent{ BONSAI_ENGINE_EVENT_TYPE_QUIT });
                break;
            default:
                break;
            }
        }
    }
} // namespace bonsai::subsystems
