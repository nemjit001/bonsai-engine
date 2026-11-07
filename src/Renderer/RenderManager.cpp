#include "RenderManager.hpp"

#include "Core/Logger.hpp"

namespace Bonsai
{
    static constexpr char const*    WindowTitle         = "Bonsai Engine";
    static constexpr uint32_t       DefaultWindowWidth  = 1280u;
    static constexpr uint32_t       DefaultWindowHeight = 720u;

    bool RenderManager::Initialize()
    {
        SPDLOG_TRACE("Creating application window");
        // Set the window flags
        SDL_WindowFlags const windowFlags = SDL_WINDOW_RESIZABLE
            | SDL_WINDOW_HIGH_PIXEL_DENSITY
            | SDL_WINDOW_VULKAN;

        // Create the window
        _window = SDL_CreateWindow(WindowTitle, DefaultWindowWidth, DefaultWindowHeight, windowFlags);
        if (_window == nullptr)
        {
            SPDLOG_ERROR("Failed to create application window");
            return false;
        }

        // Create a graphics device
        SPDLOG_TRACE("Creating graphics device");
        _graphicsDevice = GraphicsDevice::TryCreate();
        if (_graphicsDevice == nullptr)
        {
            SPDLOG_ERROR("Failed to create graphics device");
            SDL_DestroyWindow(_window);
            return false;
        }

        // Create a swapchain
        SPDLOG_TRACE("Creating graphics swapchain");
        _swapchain = _graphicsDevice->CreateSwapchain();
        if (_swapchain == nullptr)
        {
            SPDLOG_ERROR("Failed to create graphics swapchain");
            delete _graphicsDevice;
            SDL_DestroyWindow(_window);
            return false;
        }

        return true;
    }

    void RenderManager::Shutdown()
    {
        _graphicsDevice->WaitForIdle();
        delete _swapchain;
        delete _graphicsDevice;
        SDL_DestroyWindow(_window);
    }

    bool RenderManager::StartFrame()
    {
        // Try to start a new frame
        _activeFrame = _graphicsDevice->StartFrame(&_swapchain, 1);
        if (!_activeFrame) {
            return false;
        }

        return true;
    }

    void RenderManager::RenderFrame()
    {
        // Submit the active frame
        _graphicsDevice->SubmitFrame(_activeFrame);
    }

    void RenderManager::HandlePlatformEvent([[maybe_unused]] SDL_Event const& event)
    {
        //
    }
} // namespace Bonsai
