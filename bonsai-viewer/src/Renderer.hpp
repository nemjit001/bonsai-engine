#pragma once
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "RenderBackend.hpp"

class BRenderer
{
public:
    static constexpr uint32_t FRAMES_IN_FLIGHT = 2;

    /// @brief Initialize the renderer.
    /// @param pRenderBackend 
    void Init(IRenderBackend* pRenderBackend);

    /// @brief Shut down the renderer.
    void Shutdown();

    /// @brief Render an application frame.
    void Frame();

private:
    IRenderBackend* RenderBackend = nullptr;
};

#endif //RENDERER_HPP
