#pragma once
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <BonsaiBase.hpp>
#include <BonsaiGraphics.hpp>

class Renderer
{
public:
    Renderer();
    ~Renderer();

    Renderer(Renderer const&) = delete;
    Renderer& operator=(Renderer const&) = delete;

    /// @brief Initialize the renderer.
    /// @return 
    [[nodiscard]] bool Init();

    /// @brief Shut down the renderer.
    void Shutdown();

    /// @brief Render the next frame.
    void Frame();

private:
    [[nodiscard]] bool ConfigureSurface(SDL_Window* window, WGPUSurface surface, WGPUPresentMode presentMode) const;

private:
    GraphicsDevice* _graphicsDevice = nullptr;
    SDL_Window* _mainWindow = nullptr;
    WGPUSurface _mainWindowSurface = nullptr;
};

#endif //RENDERER_HPP
