#pragma once
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <cstdint>
#include "Renderer/RenderHandle.hpp"

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
    RENDERER_RESULT_SYNC_STATE_BAD_INIT     = 0x0020,
    RENDERER_RESULT_IMGUI_BAD_INIT          = 0x0040,
};

/// @brief Available GPU command queues.
enum GPUCommandQueue : uint32_t
{
    GPUCommandQueue_Direct      = 0,    //> Support for graphics, compute, and transfer operations.
    GPUCommandQueue_Compute     = 1,    //> Support for compute and transfer operations.
    GPUCommandQueue_Transfer    = 2,    //> Support for transfer operations only.
    GPUCommandQueue_COUNT,
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
    GPUTextureType_1D       = 0,
    GPUTextureType_2D       = 1,
    GPUTextureType_3D       = 2,
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
    BShaderSource* VertexShader;
    BShaderSource* HullShader;
    BShaderSource* DomainShader;
    BShaderSource* GeometryShader;
    BShaderSource* PixelShader;

    // TODO(nemjit001): Render pipeline desc
    // - [ ] add fixed function state
};

/// @brief Init info for a render backend.
struct BRenderBackendInitInfo
{
    char const* WindowTitle;
    uint32_t WindowWidth;
    uint32_t WindowHeight;
    uint32_t NumFramesInFlight; //> Number of frames that might overlap during command recording.
};

/// @brief A GPUCommandList can be used to record rendering commands for a given renderer backend.
/// Command list creation and submission is queue-specific.
class IGPUCommandList
{
public:
    IGPUCommandList() = default;
    virtual ~IGPUCommandList() = default;

    IGPUCommandList(IGPUCommandList const&) = delete;
    IGPUCommandList& operator=(IGPUCommandList const&) = delete;

    IGPUCommandList(IGPUCommandList&&) = default;
    IGPUCommandList& operator=(IGPUCommandList&&) = default;

    /// @brief Reset the command list.
    /// @return 
    virtual bool Reset() = 0;

    /// @brief Close the command list.
    /// @return 
    virtual bool Close() = 0;

    /// @brief Set the active PSO.
    /// @param PipelineState 
    virtual void SetPipelineState(BGPUPipelineStateHandle PipelineState) = 0;

    /// @brief Dispatch a bound pipeline state as compute workload.
    /// @param X 
    /// @param Y 
    /// @param Z 
    virtual void Dispatch(uint32_t X, uint32_t Y, uint32_t Z) = 0;
};

/// @brief A RenderBackend implements a graphics backend that handles the implementation of a low level rendering API.
class IRenderBackend
{
public:
    IRenderBackend() = default;
    virtual ~IRenderBackend() = default;

    IRenderBackend(IRenderBackend const&) = delete;
    IRenderBackend& operator=(IRenderBackend const&) = delete;

    IRenderBackend(IRenderBackend&&) = default;
    IRenderBackend& operator=(IRenderBackend&&) = default;

    /// @brief Initialize the renderer.
    /// @param InitInfo Initialization info.
    /// @return 
    virtual RendererResult Init(BRenderBackendInitInfo InitInfo) = 0;

    /// @brief Shut down the renderer.
    virtual void Shutdown() = 0;

    /// @brief Start a new frame in the renderer.
    virtual void NewFrame() = 0;

    /// @brief Present the active renderer frame.
    virtual void Present() = 0;

    /// @brief Get the current frame index.
    /// @return 
    virtual uint64_t GetFrameIndex() = 0;

    /// @brief Execute command lists on a given queue. Queues are internally synchronized.
    /// @param CommandQueue 
    /// @param CommandListCount 
    /// @param CommandLists 
    virtual void Execute(GPUCommandQueue CommandQueue, uint32_t CommandListCount, IGPUCommandList** CommandLists) = 0;

    /// @brief Create a new GPU command list.
    /// @return 
    virtual IGPUCommandList* CreateCommandList() = 0;

    /// @brief Destroy a GPU command list.
    /// @param CommandList 
    virtual void DestroyCommandList(IGPUCommandList* CommandList) = 0;

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

    /// @brief Get the local dispatch size for a pipeline state object.
    /// @param PipelineState 
    /// @return 
    virtual BGPUPipelineLocalSize GetPipelineLocalSize(BGPUPipelineStateHandle PipelineState) = 0;
};

#endif //RENDERER_HPP
