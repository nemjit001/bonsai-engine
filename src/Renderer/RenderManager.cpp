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
        GraphicsSwapchainDesc swapchainDesc{};
        swapchainDesc.window = _window;
        swapchainDesc.width = DefaultWindowWidth;
        swapchainDesc.height = DefaultWindowHeight;
        swapchainDesc.syncInterval = 0;
        swapchainDesc.allowTearing = false;

        _swapchain = _graphicsDevice->CreateSwapchain(swapchainDesc);
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
        _graphicsDevice->DestroySwapchain(_swapchain);
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
        // Record frame commands
        GraphicsCommandBuffer* commandBuffer = _graphicsDevice->BeginGraphicsCommandBuffer(_activeFrame);
        commandBuffer->ClearRenderTarget(nullptr, GraphicsClearColorValue{{ 0.0F, 0.0F, 0.0F, 0.0F }}); // TODO(nemjit001): Render to swap backbuffer

        // Submit the active frame
        _graphicsDevice->SubmitFrame(_activeFrame);
    }

    void RenderManager::HandlePlatformEvent(SDL_Event const& event)
    {
        if (event.type == SDL_EVENT_WINDOW_RESIZED)
        {
            uint32_t const width = event.window.data1;
            uint32_t const height = event.window.data2;
            _graphicsDevice->ResizeSwapchain(_swapchain, width, height);
        }
        else if (event.type == SDL_EVENT_WINDOW_RESTORED)
        {
            int32_t width = 0;
            int32_t height = 0;
            SDL_GetWindowSizeInPixels(_window, &width, &height);
            _graphicsDevice->ResizeSwapchain(_swapchain, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
        }
    }
} // namespace Bonsai
