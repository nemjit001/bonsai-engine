#pragma once
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <cstdint>
#include <vector>

class BGPUBuffer;
class BGPUTexture;
class BGPUPipelineState;

/// @brief Renderer return codes.
enum RendererResult : uint32_t
{
    RENDERER_RESULT_OK                      = 0x0000,
    RENDERER_RESULT_SYMBOL_LOADING_BAD_INIT = 0x0001,
    RENDERER_RESULT_WINDOW_BAD_INIT         = 0x0002,
    RENDERER_RESULT_GFX_API_BAD_INIT        = 0x0004,
    RENDERER_RESULT_UNSUPPORTED_GFX_DEVICE  = 0x0008,
    RENDERER_RESULT_SWAPCHAIN_BAD_INIT      = 0x0010,
    RENDERER_RESULT_FRAME_STATE_BAD_INIT    = 0x0020,
    RENDERER_RESULT_IMGUI_BAD_INIT          = 0x0040,
};

/// @brief Available GPU heap types.
enum GPUHeapType : uint32_t
{
    GPUHeapType_Default     = 1,
    GPUHeapType_Upload      = 2,
    GPUHeapType_Readback    = 3,
};

/// @brief Available GPU format types.
enum GPUFormat : uint32_t
{
    GPUFormat_Unknown = 0,

    GPUFormat_R8_UNORM,
    GPUFormat_R8_SNORM,
    GPUFormat_R8_UINT,
    GPUFormat_R8_SINT,

    GPUFormat_RG8_UNORM,
    GPUFormat_RG8_SNORM,
    GPUFormat_RG8_UINT,
    GPUFormat_RG8_SINT,

    GPUFormat_BGR8_UNORM,
    GPUFormat_BGR8_SNORM,
    GPUFormat_BGR8_UINT,
    GPUFormat_BGR8_SINT,
    GPUFormat_BGR8_SRGB,

    GPUFormat_RGBA8_UNORM,
    GPUFormat_RGBA8_SNORM,
    GPUFormat_RGBA8_UINT,
    GPUFormat_RGBA8_SINT,
    GPUFormat_RGBA8_SRGB,

    GPUFormat_BGRA8_UNORM,
    GPUFormat_BGRA8_SNORM,
    GPUFormat_BGRA8_UINT,
    GPUFormat_BGRA8_SINT,
    GPUFormat_BGRA8_SRGB,

    GPUFormat_R16_UNORM,
    GPUFormat_R16_SNORM,
    GPUFormat_R16_UINT,
    GPUFormat_R16_SINT,
    GPUFormat_R16_SFLOAT,

    GPUFormat_RG16_UNORM,
    GPUFormat_RG16_SNORM,
    GPUFormat_RG16_UINT,
    GPUFormat_RG16_SINT,
    GPUFormat_RG16_SFLOAT,

    GPUFormat_RGB16_UNORM,
    GPUFormat_RGB16_SNORM,
    GPUFormat_RGB16_UINT,
    GPUFormat_RGB16_SINT,
    GPUFormat_RGB16_SFLOAT,

    GPUFormat_RGBA16_UNORM,
    GPUFormat_RGBA16_SNORM,
    GPUFormat_RGBA16_UINT,
    GPUFormat_RGBA16_SINT,
    GPUFormat_RGBA16_SFLOAT,

    GPUFormat_R32_UINT,
    GPUFormat_R32_SINT,
    GPUFormat_R32_SFLOAT,

    GPUFormat_RG32_UINT,
    GPUFormat_RG32_SINT,
    GPUFormat_RG32_SFLOAT,

    GPUFormat_RGB32_UINT,
    GPUFormat_RGB32_SINT,
    GPUFormat_RGB32_SFLOAT,

    GPUFormat_RGBA32_UINT,
    GPUFormat_RGBA32_SINT,
    GPUFormat_RGBA32_SFLOAT,

    GPUFormat_D16_UNORM,
    GPUFormat_D32_SFLOAT,
    GPUFormat_S8_UINT,
    GPUFormat_D16_UNORM_S8_UINT,
    GPUFormat_D24_UNORM_S8_UINT,
    GPUFormat_D32_SFLOAT_S8_UINT,
};

/// @brief Available renderer texture types.
enum GPUTextureType : uint32_t
{
    TextureType_1D  = 1,
    TextureType_2D  = 2,
    TextureType_3D  = 3,
};

/// @brief Flags specifying GPU buffer usage.
enum GPUBufferUsage : uint32_t
{
    GPUBufferUsage_TransferSrc          = 0x00000001,
    GPUBufferUsage_TransferDst          = 0x00000002,
    GPUBufferUsage_UniformTexelBuffer   = 0x00000004,
    GPUBufferUsage_StorageTexelBuffer   = 0x00000008,
    GPUBufferUsage_UniformBuffer        = 0x00000010,
    GPUBufferUsage_StorageBuffer        = 0x00000020,
    GPUBufferUsage_IndexBuffer          = 0x00000040,
    GPUBufferUsage_VertexBuffer         = 0x00000080,
    GPUBufferUsage_IndirectBuffer       = 0x00000100,
};
typedef uint32_t GPUBufferUsageFlags;

/// @brief Flags specifying GPU texture usage.
enum GPUTextureUsage : uint32_t
{
    GPUTextureUsage_TransferSrc         = 0x00000001,
    GPUTextureUsage_TransferDst         = 0x00000002,
    GPUTextureUsage_Sampled             = 0x00000004,
    GPUTextureUsage_Storage             = 0x00000008,
    GPUTextureUsage_RenderTarget        = 0x00000010,
    GPUTextureUsage_DepthStencilTarget  = 0x00000020,
};
typedef uint32_t GPUTextureUsageFlags;

/// @brief A GPUBufferDesc is used to describe a GPU buffer on creation.
struct BGPUBufferDesc
{
    uint32_t Size;
    GPUBufferUsageFlags BufferUsage;
};

/// @brief A BGPUTextureDesc is used to describe a GPU texture on creation.
struct BGPUTextureDesc
{
    GPUTextureType TextureType;
    GPUFormat TextureFormat;
    uint32_t Width;
    uint32_t Height;
    uint32_t DepthOrLayers;
    uint32_t MipLevels;
    uint32_t SampleCount;
    GPUTextureUsageFlags TextureUsage;
};

/// @brief A BGPUComputePipelineStateDesc is used to describe a compute pipeline state on creation.
struct BGPUComputePipelineStateDesc
{
    // TODO(nemjit001): Compute pipeline desc
    // - [ ] add shader layout (reflection might be easier, would require own compilation logic)
    // - [ ] add shader data, either bytecode or raw shader source(s) compiled by backend
};

/// @brief A BGPURenderPipelineStateDesc is used to describe a render pipeline state on creation.
struct BGPURenderPipelineStateDesc
{
    // TODO(nemjit001): Render pipeline desc
    // - [ ] add shader data (see compute pipeline TODOs)
    // - [ ] add fixed function configuration state
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
    virtual TRenderHandle<BGPUBuffer> CreateBuffer(GPUHeapType HeapType, BGPUBufferDesc BufferDesc) = 0;
    
    /// @brief Destroy a GPU buffer.
    /// @param Buffer 
    virtual void DestroyBuffer(TRenderHandle<BGPUBuffer> Buffer) = 0;

    /// @brief Create a new GPU texture.
    /// @param TextureDesc 
    /// @return 
    virtual TRenderHandle<BGPUTexture> CreateTexture(GPUHeapType HeapType, BGPUTextureDesc TextureDesc) = 0;

    /// @brief Destroy a GPU texture.
    /// @param Texture 
    virtual void DestroyTexture(TRenderHandle<BGPUTexture> Texture) = 0;

    /// @brief Create a new GPU compute pipeline state.
    /// @param PipelineDesc 
    /// @return 
    virtual TRenderHandle<BGPUPipelineState> CreateComputePipelineState(BGPUComputePipelineStateDesc PipelineDesc) = 0;

    /// @brief Create a new GPU render pipeline state.
    /// @param PipelineDesc 
    /// @return 
    virtual TRenderHandle<BGPUPipelineState> CreateRenderPipelineState(BGPURenderPipelineStateDesc PipelineDesc) = 0;

    /// @brief Destroy a GPU pipeline state object.
    /// @param PipelineState 
    virtual void DestroyPipelineState(TRenderHandle<BGPUPipelineState> PipelineState) = 0;
};

#include "Renderer.inl"
#endif //RENDERER_HPP
