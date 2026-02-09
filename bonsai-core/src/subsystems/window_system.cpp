#include "bonsai/subsystems/window_system.hpp"

#include <SDL3/SDL.h>

namespace bonsai::subsystems
{
    bool WindowSystem::init()
    {
        return SDL_Init(SDL_INIT_VIDEO);
    }

    void WindowSystem::shutdown()
    {
        SDL_Quit();
    }

    void WindowSystem::run()
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

    WindowID WindowSystem::create_window(char const* title, int32_t width, int32_t height)
    {
        SDL_Window* window = SDL_CreateWindow(title, width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
        if (window == nullptr)
        {
            return BONSAI_INVALID_WINDOW_ID;
        }

        return SDL_GetWindowID(window);
    }

    void WindowSystem::destroy_window(WindowID window_id)
    {
        SDL_Window* window = SDL_GetWindowFromID(window_id);
        if (window != nullptr)
        {
            SDL_DestroyWindow(window);
        }
    }
} // namespace bonsai::subsystems
