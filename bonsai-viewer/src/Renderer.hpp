#pragma once
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <cstdint>
#include "Assets/StaticMesh.hpp"

class BGPUBuffer;       // Storage or uniform buffer representation used by the render backend.
class BGPUTexture;      // Texture representation used by the render backend.
class BPipelineState;   // Pipeline state used by the render backend.
class BGPUStaticMesh;   // Static mesh representation used by the render backend.

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

/// @brief Typed render handle for managed renderer resouces.
/// @tparam RenderType 
template<class RenderType>
struct TRenderHandle
{
    uint32_t ResourceId;
};

/// @brief A Renderer implements a graphics backend that handles the implementation of a low level rendering API.
class BRenderer
{
public:
    BRenderer() = default;
    virtual ~BRenderer() = default;

    BRenderer(BRenderer const&) = delete;
    BRenderer& operator=(BRenderer const&) = delete;

    BRenderer(BRenderer&&) = default;
    BRenderer& operator=(BRenderer&&) = default;

    /// @brief Initialize the renderer.
    /// @return 
    virtual RendererResult Init() = 0;

    /// @brief Shut down the renderer.
    virtual void Shutdown() = 0;

    /// @brief Render a new frame.
    virtual void Frame() = 0;
};

#endif //RENDERER_HPP
