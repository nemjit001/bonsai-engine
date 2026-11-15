#pragma once
#ifndef BONSAI_RENDER_MANAGER_HPP
#define BONSAI_RENDER_MANAGER_HPP

#include <SDL3/SDL.h>
#include "Gfx/GraphicsDevice.hpp"

namespace Bonsai
{
    /// @brief The RenderManager manages high level render resource management and frame execution.
    class RenderManager
    {
    public:
        /// @brief Initialize the render manager.
        /// @return A boolean indicating successful initialization.
        bool Initialize();

        /// @brief Shut down the render manager.
        void Shutdown();

        /// @brief Start a frame.
        /// @return A boolean indicating successful frame start.
        bool StartFrame();

        /// @brief Record the render commands for the current frame.
        void RenderFrame();

        /// @brief Handle a platform event.
        /// @param event 
        void HandlePlatformEvent(SDL_Event const& event);

        /// @brief Get the graphics device for the render manager.
        /// @return The stored graphics device.
        GraphicsDevice* GetGraphicsDevice() { return _graphicsDevice; }

        /// @brief Get the current window resolution.
        /// @return The current window resolution in pixels.
        Extent2D GetWindowResolution() const;

    private:
        SDL_Window* _window = nullptr;
        GraphicsDevice* _graphicsDevice = nullptr;
        GraphicsSwapchain* _swapchain = nullptr;
        FrameContext* _activeFrame = nullptr;
    };
} // namespace Bonsai

#endif //BONSAI_RENDER_MANAGER_HPP
