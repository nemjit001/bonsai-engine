#pragma once
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <cstdint>

/// @brief Typed render handle for managed renderer resouces.
/// @tparam RenderType 
template<class RenderType>
struct TRenderHandle
{
    uint32_t ResourceId : 24;
    uint32_t ResourceVersion : 8;
};

class BGPUBuffer;
using BGPUBufferHandle = TRenderHandle<BGPUBuffer>;

class BGPUTexture;
using BGPUTextureHandle = TRenderHandle<BGPUTexture>;

class BGPUPipelineState;
using BGPUPipelineStateHandle = TRenderHandle<BGPUPipelineState>;

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
    GPUHeapType_Default     = 0,
    GPUHeapType_Upload      = 1,
    GPUHeapType_Readback    = 2,
    GPUHeapType_COUNT,
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

    GPUFormat_COUNT,
};

/// @brief Available renderer texture types.
enum GPUTextureType : uint32_t
{
    GPUTextureType_1D  = 0,
    GPUTextureType_2D  = 1,
    GPUTextureType_3D  = 2,
    GPUTextureType_COUNT,
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

/// @brief Flags specifying shader stages.
enum ShaderStage : uint32_t
{
    ShaderStage_Vertex      = 0x00000001,
    ShaderStage_Hull        = 0x00000002,
    ShaderStage_Domain      = 0x00000004,
    ShaderStage_Geometry    = 0x00000008,
    ShaderStage_Pixel       = 0x00000010,
    ShaderStage_Compute     = 0x00000020,
};
typedef uint32_t ShaderStageFlags;

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

/// @brief A BShaderSource is used as input for a shader stage in a pipeline state object.
/// A ShaderSource structure will be compiled into a backend-specific representation.
struct BShaderSource
{
    ShaderStage Stage;
    char const* Entrypoint;
    char const* ShaderPath;
};

/// @brief A GPUPipelineLocalSize is used to store the local dispatch size of GPGPU workloads.
/// This can be queried for pipelines after creation.
struct BGPUPipelineLocalSize
{
    uint32_t SizeX;
    uint32_t SizeY;
    uint32_t SizeZ;
};

/// @brief A BGPUComputePipelineStateDesc is used to describe a compute pipeline state on creation.
struct BGPUComputePipelineStateDesc
{
    BShaderSource ComputeShader;
};

/// @brief A BGPURenderPipelineStateDesc is used to describe a render pipeline state on creation.
struct BGPURenderPipelineStateDesc
{
    // TODO(nemjit001): Render pipeline desc
    // - [ ] add graphics pipeline shader data
    // - [ ] add fixed function state
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
    virtual BGPUBufferHandle CreateBuffer(GPUHeapType HeapType, BGPUBufferDesc BufferDesc) = 0;
    
    /// @brief Destroy a GPU buffer.
    /// @param Buffer 
    virtual void DestroyBuffer(BGPUBufferHandle Buffer) = 0;

    /// @brief Create a new GPU texture.
    /// @param TextureDesc 
    /// @return 
    virtual BGPUTextureHandle CreateTexture(GPUHeapType HeapType, BGPUTextureDesc TextureDesc) = 0;

    /// @brief Destroy a GPU texture.
    /// @param Texture 
    virtual void DestroyTexture(BGPUTextureHandle Texture) = 0;

    /// @brief Create a new GPU compute pipeline state.
    /// @param PipelineDesc 
    /// @return 
    virtual BGPUPipelineStateHandle CreateComputePipelineState(BGPUComputePipelineStateDesc PipelineDesc) = 0;

    /// @brief Create a new GPU render pipeline state.
    /// @param PipelineDesc 
    /// @return 
    virtual BGPUPipelineStateHandle CreateRenderPipelineState(BGPURenderPipelineStateDesc PipelineDesc) = 0;

    /// @brief Destroy a GPU pipeline state object.
    /// @param PipelineState 
    virtual void DestroyPipelineState(BGPUPipelineStateHandle PipelineState) = 0;
};

#endif //RENDERER_HPP
