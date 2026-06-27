#include "RenderManager.hpp"

bool RenderManager::Init(RenderManagerInitInfo const& initInfo)
{
    _window = SDL_CreateWindow(
        initInfo.windowTitle.data(),
        initInfo.windowWidth,
        initInfo.windowHeight,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY
    );
    if (!_window) {
        return false;
    }

    return true;
}

void RenderManager::Shutdown()
{
    SDL_DestroyWindow(_window);
}

void RenderManager::NewFrame()
{
    //
}

void RenderManager::EndFrame()
{
    //
}

void RenderManager::ProcessEvent([[maybe_unused]] SDL_Event const& event)
{
    //
}
