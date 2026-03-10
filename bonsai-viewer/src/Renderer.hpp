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

/// @brief Typed resource handle for internal render resources. Used by resource pools to track internal render resources.
/// @tparam RenderType 
template<class RenderType>
struct TRenderResource
{
    bool IsFree;
    RenderType Resource;
    TRenderHandle<RenderType> Handle;
};

/// @brief Typed resource pool for internally tracking renderer resources.
/// @tparam RenderType 
template<class RenderType>
class TRenderResourcePool
{
public:
    /// @brief Allocate a new render resource from the resource pool.
    /// @return 
    TRenderResource<RenderType> Allocate()
    {
        TRenderResource<RenderType> Resource{};
        Resource.IsFree = false;
        Resource.Resource = {};
        Resource.Handle = { static_cast<uint32_t>(Resources.size()), 0 };

        Resources.push_back(Resource);
        return Resource;
    }

    /// @brief Free a render resource from the resource pool.
    /// @param Resource 
    void Free(TRenderResource<RenderType> Resource)
    {
        if (Resource.Handle.ResourceId >= Resources.size()) {
            return;
        }

        TRenderResource<RenderType>& Internal = Resources[Resource.Handle.ResourceId];
        Internal.IsFree = true;
        Internal.Resource = {};
        Internal.Handle.ResourceVersion += 1;
    }

    /// @brief Get a resource by handle from a resource pool.
    /// @param Handle 
    /// @return 
    TRenderResource<RenderType> GetResource(TRenderHandle<RenderType> Handle)
    {
        if (Handle.ResourceId >= Resources.size()) {
            return {};
        }

        TRenderResource<RenderType> Resource = Resources[Handle.ResourceId];
        if (Handle.ResourceVersion != Resource.Handle.ResourceVersion) {
            return {};
        }

        return Resource;
    }

private:
    std::vector<TRenderResource<RenderType>> Resources;
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

    virtual TRenderHandle<BGPUBuffer> CreateBuffer(BGPUBufferDesc BufferDesc) = 0;
    
    virtual void DestroyBuffer(TRenderHandle<BGPUBuffer> Buffer) = 0;

    virtual TRenderHandle<BGPUTexture> CreateTexture(BGPUTextureDesc TextureDesc) = 0;

    virtual void DestroyTexture(TRenderHandle<BGPUTexture> Texture) = 0;
};

#endif //RENDERER_HPP
