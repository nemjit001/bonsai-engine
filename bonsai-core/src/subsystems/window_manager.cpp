#include "bonsai/subsystems/window_manager.hpp"

#include <SDL3/SDL.h>
#include "bonsai/subsystems/engine_event_manager.hpp"
#include "bonsai/subsystems/engine_event.hpp"

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

    WindowID WindowManager::create_window(char const* title, uint32_t width, uint32_t height, WindowConfig const& config)
    {
        SDL_WindowFlags flags = 0;
        if (config.resizable) {
            flags |= SDL_WINDOW_RESIZABLE;
        }
        if (config.allow_high_dpi) {
            flags |= SDL_WINDOW_HIGH_PIXEL_DENSITY;
        }

        switch (config.client_api)
        {
        case WindowClientAPI::Vulkan:
            flags |= SDL_WINDOW_VULKAN;
            break;
        default:
            break;
        }

        SDL_Window* window = SDL_CreateWindow(title, width, height, flags);
        return SDL_GetWindowID(window);
    }

    void WindowManager::destroy_window(WindowID window)
    {
        SDL_DestroyWindow(SDL_GetWindowFromID(window));
    }

    void WindowManager::get_window_size(WindowID window, uint32_t& width, uint32_t& height)
    {
        int w = 0, h = 0;
        SDL_GetWindowSizeInPixels(SDL_GetWindowFromID(window), &w, &h);
        width = w;
        height = h;
    }
} // namespace bonsai::subsystems
