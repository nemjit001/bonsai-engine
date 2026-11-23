#pragma once
#ifndef BONSAI_RENDER_MANAGER_HPP
#define BONSAI_RENDER_MANAGER_HPP

#include <SDL3/SDL.h>
#include <Gfx/GraphicsDevice.hpp>
#include <Gfx/ShaderCompiler.hpp>
#include "Renderer/RenderWorld.hpp"

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

        /// @brief Get the active render world.
        /// @return The render world.
        RenderWorld& GetRenderWorld() { return _renderWorld; }

        /// @brief Get the current window resolution.
        /// @return The current window resolution in pixels.
        Extent2D GetWindowResolution() const;

    private:
        void HandleWindowResize(Extent2D const& windowSize);

        /// @brief Create render targets.
        /// @param extent Render target extent.
        /// @return A boolean indicating success.
        bool CreateRenderTargets(Extent2D const& extent);

        /// @brief Create render pipelines.
        /// @param shaderCompiler Shader compiler to use for shader compilation.
        /// @return A boolean indicating success.
        bool CreatePipelines(ShaderCompiler* shaderCompiler);

        /// @brief Destroy a render world and clean up resources.
        /// @param renderWorld Render world to destroy.
        void DestroyRenderWorld(RenderWorld const& renderWorld);

        /// @brief Destroy render targets.
        void DestroyRenderTargets();

        /// @brief Destroy render pipelines.
        void DestroyPipelines();

    private:
        SDL_Window* _window = nullptr;
        GraphicsDevice* _graphicsDevice = nullptr;
        GraphicsSwapchain* _swapchain = nullptr;
        FrameContext* _activeFrame = nullptr;
        RenderWorld _renderWorld; // TODO(nemjit001): buffer render worlds on frame in flight count

        //-- Render targets --//
        GraphicsTexture* _colorTarget = nullptr;
        GraphicsTexture* _depthStencilTarget = nullptr;
        GraphicsTextureView* _colorTargetView = nullptr;
        GraphicsTextureView* _depthStencilTargetView = nullptr;

        //-- Render pipelines --//
        GraphicsDescriptorSetLayout* _sceneDataDescriptorSetLayout = nullptr;
        GraphicsPipelineLayout* _renderPipelineLayout = nullptr;
        GraphicsRenderPipeline* _unlitRenderPipeline = nullptr;

        GraphicsSampler* _fullscreenSampler = nullptr;
        GraphicsDescriptorSetLayout* _presentDescriptorSetLayout = nullptr;
        GraphicsPipelineLayout* _presentPipelineLayout = nullptr;
        GraphicsRenderPipeline* _presentRenderPipeline = nullptr;
    };
} // namespace Bonsai

#endif //BONSAI_RENDER_MANAGER_HPP
