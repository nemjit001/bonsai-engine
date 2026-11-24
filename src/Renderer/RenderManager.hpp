#pragma once
#ifndef BONSAI_RENDER_MANAGER_HPP
#define BONSAI_RENDER_MANAGER_HPP

#include <SDL3/SDL.h>
#include <Gfx/GraphicsTypes.hpp>
#include "Renderer/RenderWorld.hpp"
#include "Renderer/Subrenderers/GeometryRenderer.hpp"
#include "Renderer/Subrenderers/PresentRenderer.hpp"

namespace Bonsai
{
    class ShaderCompiler;

    /// @brief The RenderTarget contains a texture/view pair for rendering to.
    struct RenderTarget
    {
        GraphicsTexture* target;
        GraphicsTextureView* targetView;
    };

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

        /// @brief Get the current window resolution.
        /// @return The current window resolution in pixels.
        Extent2D GetWindowResolution() const;

        /// @brief Get the graphics device for the render manager.
        /// @return The stored graphics device.
        GraphicsDevice* GetGraphicsDevice() { return _graphicsDevice; }

        /// @brief Get the swapchain for the render manager.
        /// @return The stored swapchain.
        GraphicsSwapchain* GetSwapchain() { return _swapchain; }

        /// @brief Get the color target for rendering.
        /// @return The color target.
        RenderTarget const& GetColorTarget() const { return _colorTarget; }

        /// @brief Get the depth stencil target for rendering.
        /// @return The depth stencil target.
        RenderTarget const& GetDepthStencilTarget() const { return _depthStencilTarget; }

        /// @brief Get the active render world.
        /// @return The render world.
        RenderWorld& GetRenderWorld() { return _renderWorld; }

    private:
        /// @brief Handle a window resize in the render manager.
        /// @param windowSize New window size.
        void HandleWindowResize(Extent2D const& windowSize);

        /// @brief Create render targets.
        /// @param extent Render target extent.
        /// @return A boolean indicating success.
        bool CreateRenderTargets(Extent2D const& extent);

        /// @brief Destroy a render world and clean up resources.
        /// @param renderWorld Render world to destroy.
        void DestroyRenderWorld(RenderWorld const& renderWorld);

        /// @brief Destroy render targets.
        void DestroyRenderTargets();

    private:
        SDL_Window* _window = nullptr;
        GraphicsDevice* _graphicsDevice = nullptr;
        GraphicsSwapchain* _swapchain = nullptr;
        FrameContext* _activeFrame = nullptr;
        RenderWorld _renderWorld; // TODO(nemjit001): buffer render worlds on frame in flight count?

        //-- Render targets --//
        RenderTarget _colorTarget;
        RenderTarget _depthStencilTarget;

        //-- Sub renderers --//
        GeometryRenderer _geometryRenderer;
        PresentRenderer _presentRenderer;
    };
} // namespace Bonsai

#endif //BONSAI_RENDER_MANAGER_HPP
