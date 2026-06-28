#include "RenderDeviceManager.hpp"

bool RenderDeviceManager::Init(RenderDeviceManagerInitInfo const& initInfo)
{
    // Initialize default window
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

void RenderDeviceManager::Shutdown()
{
    SDL_DestroyWindow(_window);
}

void RenderDeviceManager::NewFrame()
{
    //
}

void RenderDeviceManager::EndFrame()
{
    //
}

void RenderDeviceManager::ProcessEvent([[maybe_unused]] SDL_Event const& event)
{
    //
}
