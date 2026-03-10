#pragma once
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <cstdint>
#include <vector>

class BGPUBuffer;
class BGPUTexture;

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
    RENDERER_RESULT_IMGUI_BAD_INIT          = 0x70,
};

/// @brief A GPUBufferDesc is used to describe a newly created GPU buffer.
struct BGPUBufferDesc
{
    size_t Size;
};

/// @brief A BGPUTextureDesc is used to describe a newly created GPU texture.
struct BGPUTextureDesc
{

};

/// @brief Typed render handle for managed renderer resouces.
/// @tparam RenderType 
template<class RenderType>
struct TRenderHandle
{
    uint32_t ResourceId         : 24;
    uint32_t ResourceVersion    : 8;
};

/// @brief Typed resource pool for internally tracking renderer resources.
/// @tparam RenderType 
template<class RenderType>
class TRenderResourcePool
{
public:
    /// @brief Create an invalid resource handle.
    /// @return 
    static constexpr TRenderHandle<RenderType> CreateInvalidHandle();

    /// @brief Allocate a new render resource from the resource pool.
    /// @param NewResource
    /// @return 
    TRenderHandle<RenderType> Allocate(RenderType NewResource);

    /// @brief Free a render resource from the resource pool.
    /// @param Handle Resource handle.
    void Free(TRenderHandle<RenderType> Handle);

    /// @brief Get a resource by handle from a resource pool.
    /// @param Handle Resource handle.
    /// @param OutResource Output resource.
    /// @return 
    bool GetResource(TRenderHandle<RenderType> Handle, RenderType& OutResource);

    /// @brief Check if a resource with a given handle is valid.
    /// @param Handle
    /// @return 
    bool IsValidResource(TRenderHandle<RenderType> Handle);

private:
    std::vector<RenderType> Resources;
    std::vector<uint32_t> ResourceVersions;
    std::vector<uint32_t> FreeList;
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

    /// @brief Create a new GPU buffer.
    /// @param BufferDesc 
    /// @return 
    virtual TRenderHandle<BGPUBuffer> CreateBuffer(BGPUBufferDesc BufferDesc) = 0;
    
    /// @brief Destroy a GPU buffer.
    /// @param Buffer 
    virtual void DestroyBuffer(TRenderHandle<BGPUBuffer> Buffer) = 0;

    /// @brief Create a new GPU texture.
    /// @param TextureDesc 
    /// @return 
    virtual TRenderHandle<BGPUTexture> CreateTexture(BGPUTextureDesc TextureDesc) = 0;

    /// @brief Destroy a GPU texture.
    /// @param Texture 
    virtual void DestroyTexture(TRenderHandle<BGPUTexture> Texture) = 0;
};

#include "Renderer.inl"
#endif //RENDERER_HPP
