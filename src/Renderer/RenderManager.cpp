#include "RenderManager.hpp"

namespace Bonsai
{
    static constexpr char const*    WindowTitle         = "Bonsai Engine";
    static constexpr uint32_t       DefaultWindowWidth  = 1280u;
    static constexpr uint32_t       DefaultWindowHeight = 720u;

    bool RenderManager::Initialize()
    {
        // Create a graphics device
        _graphicsDevice = GraphicsDevice::TryCreate();
        if (_graphicsDevice == nullptr) {
            return false;
        }

        // Set the window flags
        SDL_WindowFlags const windowFlags = SDL_WINDOW_RESIZABLE
            | SDL_WINDOW_HIGH_PIXEL_DENSITY
            | SDL_WINDOW_VULKAN;

        // Create the window
        _window = SDL_CreateWindow(WindowTitle, DefaultWindowWidth, DefaultWindowHeight, windowFlags);
        if (_window == nullptr) {
            return false;
        }

        return true;
    }

    void RenderManager::Shutdown()
    {
        SDL_DestroyWindow(_window);
        delete _graphicsDevice;
    }

    bool RenderManager::StartFrame()
    {
        return true;
    }

    void RenderManager::RenderFrame()
    {
        //
    }

    void RenderManager::HandlePlatformEvent([[maybe_unused]] SDL_Event const& event)
    {
        //
    }
} // namespace Bonsai
