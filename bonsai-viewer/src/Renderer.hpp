#pragma once
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <memory>
#include "Renderer/RenderHandle.hpp"

class BPipelineState;
class BGPUStaticMesh;

/// @brief Renderer backend implementation.
struct BRendererImpl;

/// @brief Renderer return codes.
enum RendererResult : int
{
    RENDERER_RESULT_OK                      = 0x00,
    RENDERER_RESULT_SYMBOL_LOADING_BAD_INIT = 0x10,
    RENDERER_RESULT_WINDOW_BAD_INIT         = 0x20,
    RENDERER_RESULT_GFX_API_BAD_INIT        = 0x30,
    RENDERER_RESULT_UNSUPPORTED_GFX_DEVICE  = 0x40,
    RENDERER_RESULT_SWAPCHAIN_BAD_INIT      = 0x50,
    RENDERER_RESULT_FRAME_STATE_BAD_INIT    = 0x60,
};

/// @brief A Renderer implements a graphics backend that handles the implementation of a low level rendering API.
class BRenderer
{
public:
    BRenderer();
    ~BRenderer();

    BRenderer(BRenderer const&) = delete;
    BRenderer& operator=(BRenderer const&) = delete;

    BRenderer(BRenderer&&) = delete;
    BRenderer& operator=(BRenderer&&) = delete;

    /// @brief Initialize the renderer.
    /// @return 
    RendererResult Init();

    /// @brief Shut down the renderer.
    void Shutdown();

    /// @brief Render a new frame.
    void Frame();

private:
    std::unique_ptr<BRendererImpl> pImpl;
};

#endif //RENDERER_HPP
