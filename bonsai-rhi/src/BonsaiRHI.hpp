#pragma once
#ifndef BONSAI_RHI_HPP
#define BONSAI_RHI_HPP

#include <BonsaiBase.hpp>
#include "BonsaiRHIExport.hpp"

/// @brief Bonsai RHI.
namespace RHI
{
    class GPUBuffer;    //< Opaque GPU buffer handle.
    class GPUTexture;   //< Opaque GPU texture handle.
    class GPUSampler;   //< Opaque GPU sampler handle.
    class GPUPipeline;  //< Opaque GPU pipeline handle.

    static constexpr uint32 ALL_REMAINING_MIPS      = (~0u);
    static constexpr uint32 ALL_REMAINING_LAYERS    = (~0u);
    static constexpr float  LOD_CLAMP_NONE          = 1'000.0F;

    static constexpr uint32 MAX_RENDER_TARGETS      = 8u;

    /// @brief Enumeration of GraphicsDevice result codes.
    enum GraphicsDeviceResult : uint32
    {
        GraphicsDeviceResult_Ok         = 0,
        GraphicsDeviceResult_BadSubmit  = 1,
        GraphicsDeviceResult_BadMap     = 2,
    };

    /// @brief Enumeration of potentially supported backend graphics APIs.
    enum BackendAPI : uint32
    {
        BackendAPI_Default  = 0,
        BackendAPI_Vulkan   = 1,
        BackendAPI_D3D12    = 2,
        BackendAPI_WebGPU   = 3,
    };

    /// @brief Enumeration of GPU queue types.
    enum GPUQueueType : uint32
    {
        GPUQueueType_Direct     = 0,
        GPUQueueType_Transfer   = 1,
        GPUQueueType_Compute    = 2,
    };

    /// @brief Enumeration of GPU heap types.
    enum GPUHeapType : uint32
    {
        GPUHeapType_Default     = 0, //< The default heap is a device-visible, CPU accessible heap that may be used for uploads or direct memory writes.
        GPUHeapType_DeviceLocal = 1, //< The device local heap is a GPU-only heap that must be populated using copy commands from default heap.
        GPUHeapType_Readback    = 2, //< The readback heap can be used for CPU readback commands.
    };

    /// @brief Enumeration of texture types.
    enum TextureType : uint32
    {
        TextureType_1D  = 0,
        TextureType_2D  = 1,
        TextureType_3D  = 2,
    };

    /// @brief Enumeration of texture view types.
    enum TextureViewType : uint32
    {
        TextureViewType_1D          = 0,
        TextureViewType_2D          = 1,
        TextureViewType_3D          = 2,
        TextureViewType_Cube        = 3,
        TextureViewType_1DArray     = 4,
        TextureViewType_2DArray     = 5,
        TextureViewType_CubeArray   = 6,
    };

    /// @brief Enumeration of texture formats.
    enum TextureFormat : uint32
    {
        TextureFormat_Unknown           = 0,
        TextureFormat_R8_UNorm          = 1,
        TextureFormat_R8_SNorm          = 2,
        TextureFormat_R8_UInt           = 3,
        TextureFormat_R8_SInt           = 4,
        TextureFormat_RG8_UNorm         = 5,
        TextureFormat_RG8_SNorm         = 6,
        TextureFormat_RG8_UInt          = 7,
        TextureFormat_RG8_SInt          = 8,
        TextureFormat_RGBA8_UNorm       = 9,
        TextureFormat_RGBA8_UNormSRGB   = 10,
        TextureFormat_RGBA8_SNorm       = 11,
        TextureFormat_RGBA8_UInt        = 12,
        TextureFormat_RGBA8_SInt        = 13,
        TextureFormat_BGRA8_UNorm       = 14,
        TextureFormat_BGRA8_UNormSRGB   = 15,
        TextureFormat_R16_UInt          = 16,
        TextureFormat_R16_SInt          = 17,
        TextureFormat_R16_SFloat        = 18,
        TextureFormat_RG16_UInt         = 19,
        TextureFormat_RG16_SInt         = 20,
        TextureFormat_RG16_SFloat       = 21,
        TextureFormat_RGBA16_UInt       = 22,
        TextureFormat_RGBA16_SInt       = 23,
        TextureFormat_RGBA16_SFloat     = 24,
        TextureFormat_R32_UInt          = 25,
        TextureFormat_R32_SInt          = 26,
        TextureFormat_R32_SFloat        = 27,
        TextureFormat_RG32_UInt         = 28,
        TextureFormat_RG32_SInt         = 29,
        TextureFormat_RG32_SFloat       = 30,
        TextureFormat_RGBA32_UInt       = 31,
        TextureFormat_RGBA32_SInt       = 32,
        TextureFormat_RGBA32_SFloat     = 33,
        TextureFormat_S8                = 34,
        TextureFormat_D16_UNorm         = 35,
        TextureFormat_D24               = 36,
        TextureFormat_D24_S8            = 37,
        TextureFormat_D32_SFLoat        = 38,
        TextureFormat_D32_SFLoat_S8     = 39,
    };

    /// @brief Enumeration of filter modes.
    enum Filter : uint32
    {
        Filter_Nearest  = 0,
        Filter_Linear   = 1,
    };

    /// @brief Enumeration of sampler mipmap modes.
    enum SamplerMipmapMode : uint32
    {
        SamplerMipmapMode_Nearest   = 0,
        SamplerMipmapMode_Linear    = 1,
    };

    /// @brief Enumeration of sampler address modes.
    enum SamplerAddressMode : uint32
    {
        SamplerAddressMode_Repeat           = 0,
        SamplerAddressMode_MirroredRepeat   = 1,
        SamplerAddressMode_ClampToEdge      = 2,
        SamplerAddressMode_ClampToBorder    = 3,
    };

    /// @brief Enumeration of polygon raster modes.
    enum PolygonMode : uint32
    {
        PolygonMode_Fill    = 0,
        PolygonMode_Line    = 1,
        PolygonMode_Point   = 2,
    };

    /// @brief Enumeration of cull modes.
    enum CullMode : uint32
    {
        CullMode_None   = 0,
        CullMode_Front  = 1,
        CullMode_Back   = 2,
    };

    /// @brief Enumeration of primitive topologies
    enum PrimitiveTopology : uint32
    {
        PrimitiveTopology_Point         = 0,
        PrimitiveTopology_LineList      = 1,
        PrimitiveTopology_LineStrip     = 2,
        PrimitiveTopology_TriangleList  = 3,
        PrimitiveTopology_TriangleStrip = 4,
        PrimitiveTopology_TriangleFan   = 5,
    };

    /// @brief Enumeration of comparison functions.
    enum CompareOp : uint32
    {
        CompareOp_Never             = 0,
        CompareOp_Less              = 1,
        CompareOp_Equal             = 2,
        CompareOp_LessOrEqual       = 3,
        CompareOp_Greater           = 4,
        CompareOp_NotEqual          = 5,
        CompareOp_GreaterOrEqual    = 6,
        CompareOp_Always            = 7,
    };

    /// @brief Enumeration of blend operations.
    enum BlendOp : uint32
    {
        BlendOp_Add         = 0,
        BlendOp_Subtract    = 1,
        BlendOp_RevSubtract = 2,
        BlendOp_Min         = 3,
        BlendOp_Max         = 4,
    };

    /// @brief Enumeration of logical operations.
    enum LogicOp : uint32
    {
        LogicOp_Clear           = 0,
        LogicOp_And             = 1,
        LogicOp_AndReverse      = 2,
        LogicOp_Copy            = 3,
        LogicOp_AndInverted     = 4,
        LogicOp_NoOp            = 5,
        LogicOp_Xor             = 6,
        LogicOp_Or              = 7,
        LogicOp_Nor             = 8,
        LogicOp_Equivalent      = 9,
        LogicOp_Invert          = 10,
        LogicOp_OrReverse       = 11,
        LogicOp_CopyInverted    = 12,
        LogicOp_OrInverted      = 13,
        LogicOp_Nand            = 14,
        LogicOp_Set             = 15,
    };

    /// @brief Enumeration of load operations.
    enum LoadOp : uint32
    {
        LoadOp_Load     = 0,
        LoadOp_Clear    = 1,
        LoadOp_DontCare = 2,
    };

    /// @brief Enumeration of store operations.
    enum StoreOp : uint32
    {
        StoreOp_Store       = 0,
        StoreOp_DontCare    = 1,
    };

    /// @brief Enumeration of blend factors.
    enum BlendFactor : uint32
    {
        BlendFactor_Zero                = 0,
        BlendFactor_One                 = 1,
        BlendFactor_SrcColor            = 2,
        BlendFactor_DestColor           = 3,
        BlendFactor_SrcAlpha            = 4,
        BlendFactor_DestAlpha           = 5,
        BlendFactor_ConstantColor       = 6,
        BlendFactor_ConstantAlpha       = 7,
        BlendFactor_SrcAlphaSaturate    = 8,
        BlendFactor_Src1Color           = 9,
        BlendFactor_Src1Alpha           = 10,
        BlendFactor_InvSrcColor         = 11,
        BlendFactor_InvDestColor        = 12,
        BlendFactor_InvSrcAlpha         = 13,
        BlendFactor_InvDestAlpha        = 14,
        BlendFactor_InvConstantColor    = 15,
        BlendFactor_InvConstantAlpha    = 16,
        BlendFactor_InvSrc1Color        = 17,
        BlendFactor_InvSrc1Alpha        = 18,
    };

    /// @brief Bitflags for buffer usage.
    enum BufferUsageFlagBits : uint32
    {
        BufferUsage_TransferSrc     = 0x00000001,
        BufferUsage_TransferDst     = 0x00000002,
        BufferUsage_UniformBuffer   = 0x00000004,
        BufferUsage_StorageBuffer   = 0x00000008,
        BufferUsage_IndexBuffer     = 0x00000010,
        BufferUsage_VertexBuffer    = 0x00000020,
        BufferUsage_IndirectBuffer  = 0x00000040,
    };
    typedef uint32 BufferUsageFlags;

    /// @brief Bitflags for texture usage.
    enum TextureUsageFlagBits : uint32
    {
        TextureUsage_TransferSrc        = 0x00000001,
        TextureUsage_TransferDst        = 0x00000002,
        TextureUsage_Sampled            = 0x00000004,
        TextureUsage_Storage            = 0x00000008,
        TextureUsage_RenderTarget       = 0x00000010,
        TextureUsage_DepthStencilTarget = 0x00000020,
    };
    typedef uint32 TextureUsageFlags;
    
    /// @brief Bitflags for marking shader stages.
    enum ShaderStageFlagsBits : uint32
    {
        ShaderStage_Vertex      = 0x00000001,
        ShaderStage_Pixel       = 0x00000002,
        ShaderStage_Compute     = 0x00000004,
    };
    typedef uint32 ShaderStageFlags;

    /// @brief Bitflags for color components.
    enum ColorComponentFlagBits : uint32
    {
        ColorComponent_Red      = 0x00000001,
        ColorComponent_Green    = 0x00000002,
        ColorComponent_Blue     = 0x00000004,
        ColorComponent_Alpha    = 0x00000008,
        ColorComponent_All      = ColorComponent_Red | ColorComponent_Green | ColorComponent_Blue | ColorComponent_Alpha,
    };
    typedef uint32 ColorComponentFlags;

    /// @brief Bitflags for texture aspects.
    enum TextureAspectMaskBits : uint32
    {
        TextureAspect_Color     = 0x00000001,
        TextureAspect_Depth     = 0x00000002,
        TextureAspect_Stencil   = 0x00000004,
    };
    typedef uint32 TextureAspectMask;

    /// @brief 2D resource offset.
    struct Offset2D
    {
        int32 x;
        int32 y;
    };

    /// @brief 3D resource offset.
    struct Offset3D
    {
        int32 x;
        int32 y;
        int32 z;
    };

    /// @brief 2D resource extent.
    struct Extent2D
    {
        uint32 width;
        uint32 height;
    };

    /// @brief 3D resource extent.
    struct Extent3D
    {
        uint32 width;
        uint32 height;
        uint32 depth;
    };

    /// @brief 2D rectangle.
    struct Rect2D
    {
        Offset2D offset;
        Extent2D extent;
    };

    /// @brief Render viewport.
    struct Viewport
    {
        float x;
        float y;
        float width;
        float height;
        float minDepth;
        float maxDepth;
    };

    /// @brief Clear value for color targets.
    struct ClearColorValue
    {
        float float32[4];
        int32 int32[4];
        uint32 uint32[4];
    };

    /// @brief Clear value for depth/stencil targets.
    struct ClearDepthStencilValue
    {
        float depth;
        uint32 stencil;
    };

    /// @brief A ClearValue is used to clear render targets.
    union ClearValue
    {
        ClearColorValue color;
        ClearDepthStencilValue depthStencil;
    };

    /// @brief Indirect indexed draw command.
    struct DrawIndexedIndirectCommand
    {
        uint32 indexCount;
        uint32 instanceCount;
        uint32 firstIndex;
        int32 vertexOffset;
        uint32 firstInstance;
    };

    /// @brief Indirect dispatch command.
    struct DispatchIndirectCommand
    {
        uint32 sizeX;
        uint32 sizeY;
        uint32 sizeZ;
    };

    /// @brief A ShaderBlob contains compiled shader IR code.
    struct ShaderBlob
    {
        void* code = nullptr;
        uint32 size = 0;
    };

    /// @brief The RasterState contains rasterization fixed function state.
    struct RasterState
    {
        PolygonMode polygonMode = PolygonMode_Fill;
        PrimitiveTopology primitiveTopology = PrimitiveTopology_TriangleList;
        CullMode cullMode = CullMode_None;
        bool frontFaceCounterClockwise = false;
        uint32 sampleCount = 1;
        bool alphaToCoverageEnable = false;
        bool alphaToOneEnable = false;
    };

    /// @brief The DepthStencilState contains depth/stencil testing fixed function state.
    struct DepthStencilState
    {
        bool depthTest = false;
        bool depthWrite = false;
        CompareOp depthCompareOp = CompareOp_Always;
        float depthBias = 0.0F;
        float depthBiasSlopeFactor = 0.0F;
        float depthBiasClamp = 0.0F;
    };

    /// @brief The RenderTargetBlendState contains blend state per render target.
    struct RenderTargetBlendState
    {
        bool blendEnable = false;
        BlendFactor srcColorBlendFactor;
        BlendFactor destColorBlendFactor;
        BlendOp colorBlendOp;
        BlendFactor srcAlphaBlendFactor;
        BlendFactor destAlphaBlendFactor;
        BlendOp alphaBlendOp;
        ColorComponentFlags colorWriteMask = ColorComponent_All;
    };

    /// @brief The BlendState contains alpha blending fixed function state.
    struct BlendState
    {
        bool enableLogicOp = false;
        LogicOp logicOp = LogicOp_Clear;
        RenderTargetBlendState blendTargets[MAX_RENDER_TARGETS] = {};
    };

    /// @brief A TextureSubresourceRange represents a range of texture subresources.
    struct TextureSubresourceRange
    {
        TextureAspectMask aspectMask = 0;
        uint32 baseMipLevel = 0;
        uint32 levelCount = ALL_REMAINING_MIPS;
        uint32 baseArrayLayer = 0;
        uint32 layerCount = ALL_REMAINING_LAYERS;
    };

    /// @brief A TextureSubresource contains represents a specific texture subresource.
    struct TextureSubresource
    {
        TextureAspectMask aspectMask = 0;
        uint32 mipLevel = 0;
    };

    /// @brief A GPUBufferDesc is used to create GPUBuffers.
    struct GPUBufferDesc
    {
        GPUHeapType heapType = GPUHeapType_Default;
        uint32 size = 0;
        BufferUsageFlags usage = 0;
    };

    /// @brief A GPUTextureDesc is used to create GPUTextures.
    struct GPUTextureDesc
    {
        TextureType textureType = TextureType_1D;
        TextureFormat format = TextureFormat_Unknown;
        Extent3D size = {};
        uint32 mipLevels = 1;
        uint32 arrayLayers = 1;
        uint32 sampleCount = 1;
        TextureUsageFlags usage = 0;
    };

    /// @brief A GPUTextureViewDesc is used to create views into GPUTextures.
    struct GPUTextureViewDesc
    {
        GPUTexture* texture = nullptr;
        TextureViewType viewType = TextureViewType_1D;
        TextureFormat format = TextureFormat_Unknown;
        TextureSubresourceRange subresourceRange = {};
    };

    /// @brief A GPUSamplerDesc is used to create GPUSamplers.
    struct GPUSamplerDesc
    {
        Filter magFilter = Filter_Nearest;
        Filter minFilter = Filter_Nearest;
        SamplerMipmapMode mipmapMode = SamplerMipmapMode_Nearest;
        SamplerAddressMode addressModeU = SamplerAddressMode_Repeat;
        SamplerAddressMode addressModeV = SamplerAddressMode_Repeat;
        SamplerAddressMode addressModeW = SamplerAddressMode_Repeat;
        float mipLODBias = 0.0F;
        uint32 maxAnisotropy = 0;
        bool enableCompareOp = false;
        CompareOp compareOp = CompareOp_Never;
        float minLOD = 0.0F;
        float maxLOD = LOD_CLAMP_NONE;
    };

    /// @brief A GPUGraphicsPipelineDesc is used to create raster GPUPipelines.
    struct GPUGraphicsPipelineDesc
    {
        ShaderBlob* vertexShader = nullptr;
        ShaderBlob* pixelShader = nullptr;
        char const* vertexShaderEntrypoint = "main";
        char const* pixelShaderEntrypoint = "main";
        uint32 renderTargetCount = 0;
        TextureFormat renderTargetFormats[MAX_RENDER_TARGETS] = {};
        TextureFormat depthStencilFormat = TextureFormat_Unknown;
        RasterState rasterState = {};
        DepthStencilState* depthStencilState = nullptr;
        BlendState* blendState = nullptr;
    };

    /// @brief A GPUComputePipelineDesc is used to create compute GPUPipelines.
    struct GPUComputePipelineDesc
    {
        ShaderBlob computeShader = {};
        char const* entrypoint = "main";
    };

    /// @brief The RenderPassTargetDesc contains render pass state per attached render target.
    struct RenderPassTargetDesc
    {
        GPUTexture* texture = nullptr;
        GPUTexture* resolveTarget = nullptr;
        LoadOp loadOp = LoadOp_Load;
        StoreOp storeOp = StoreOp_Store;
        ClearValue clearValue = {};
    };

    /// @brief A RenderPassDesc is used to begin a render pass during command recording.
    struct RenderPassDesc
    {
        uint32 colorTargetCount = 0;
        RenderPassTargetDesc* colorTargets = nullptr;
        RenderPassTargetDesc* depthStencilTarget = nullptr;
    };

    /// @brief A SemVer is used to represent semantic versioning for a an app using a graphics device.
    union SemVer
    {
        struct Version
        {
            uint8 major;
            uint8 minor;
            uint8 patch;
            uint8 variant;
        } version;
        uint32 packed;
    };

    /// @brief A GraphicsDeviceDesc is used to initialize a GraphicsDevice.
    struct GraphicsDeviceDesc
    {
        char const* appName = nullptr;
        SemVer appVersion = {};
        uint32 framesInFlight = 2; //< Number of frames to process simultaneously.
        BackendAPI backendAPI = BackendAPI_Default; //< Backend API to use.
    };

    /// @brief The GPUCommandBuffer interface implements command recording on top of low-level graphics command buffers.
    class BONSAI_RHI_EXPORT IGPUCommandBuffer
    {
    public:
        virtual ~IGPUCommandBuffer() = default;

        virtual void CopyBufferToBuffer(GPUBuffer* src, GPUBuffer* dest, uint64 size, int64 srcOffset, int64 destOffset) = 0;
        virtual void CopyTextureToTexture(
            GPUTexture* src, GPUTexture* dest,
            TextureSubresource srcSubresource, Offset3D srcOffset,
            TextureSubresource destSubresource, Offset3D destOffset,
            Extent3D extent
        ) = 0;
        virtual void CopyBufferToTexture(
            GPUBuffer* src, GPUTexture* dest,
            int64 bufferOffset, uint64 rowPitch, uint64 rowCount,
            TextureSubresource subresource, Offset3D textureOffset, Extent3D textureExtent
        ) = 0;
        virtual void CopyTextureToBuffer(
            GPUTexture* src, GPUBuffer* dest,
            int64 bufferOffset, uint64 rowPitch, uint64 rowCount,
            TextureSubresource subresource, Offset3D textureOffset, Extent3D textureExtent
        ) = 0;

        virtual void SetPipeline(GPUPipeline* pipeline) = 0;
        virtual void SetDepthStencilState(DepthStencilState* depthStencilState) = 0;
        virtual void SetBlendState(BlendState* blendState) = 0;

        virtual void BeginRenderPass(RenderPassDesc* renderPassDesc) = 0;
        virtual void EndRenderPass() = 0;

        virtual void Dispatch(uint32 sizeX, uint32 sizeY, uint32 sizeZ) = 0;
        virtual void DispatchIndirect(GPUBuffer* dispatchCommands, int64 offset) = 0;

        virtual void SetViewport(Viewport* viewport) = 0;
        virtual void SetScissor(Rect2D* scissor) = 0;
        virtual void DrawIndexed(uint32 indexCount, uint32 instanceCount, uint32 firstIndex, int32 vertexOffset, uint32 firstInstance) = 0;
        virtual void DrawIndexedIndirect(GPUBuffer* drawCommands, int64 offset) = 0;

        virtual void Finish() = 0;

        virtual BackendAPI GetBackendAPI() const = 0;
    };

    /// @brief The GraphicsDevice interface implements an abstraction layer on top of low-level graphics APIs.
    class BONSAI_RHI_EXPORT IGraphicsDevice
    {
    public:
        virtual ~IGraphicsDevice() = default;

        /// @brief Create a GPU buffer.
        /// @param bufferDesc Buffer description.
        /// @return A new GPUBuffer, or nullptr on failure.
        [[nodiscard]] virtual GPUBuffer* CreateBuffer(GPUBufferDesc* bufferDesc) = 0;

        /// @brief Destroy a GPUBuffer.
        /// @param buffer 
        virtual void DestroyBuffer(GPUBuffer* buffer) = 0;

        /// @brief Create a GPU texture.
        /// @param textureDesc Texture description.
        /// @return A new GPUTexture, or nullptr on failure.
        [[nodiscard]] virtual GPUTexture* CreateTexture(GPUTextureDesc* textureDesc) = 0;

        /// @brief Create a view into a GPU texture.
        /// @param textureViewDesc Texture view description.
        /// @return A new GPUTexture as view of another GPUTexture, or nullptr on failure.
        virtual GPUTexture* CreateTextureView(GPUTextureViewDesc* textureViewDesc) = 0;

        /// @brief Destroy a GPUTexture.
        /// @param texture 
        virtual void DestroyTexture(GPUTexture* texture) = 0;

        /// @brief Create a GPU sampler.
        /// @param samplerDesc Sampler description.
        /// @return A new GPUSampler, or nullptr on failure.
        virtual GPUSampler* CreateSampler(GPUSamplerDesc* samplerDesc) = 0;

        /// @brief Destroy a GPUSampler.
        /// @param sampler 
        virtual void DestroySampler(GPUSampler* sampler) = 0;

        /// @brief Create a GPUPipeline for raster workloads.
        /// @param pipelineDesc Graphics pipeline description.
        /// @return 
        [[nodiscard]] virtual GPUPipeline* CreateGraphicsPipeline(GPUGraphicsPipelineDesc* pipelineDesc) = 0;

        /// @brief Create a GPUPipeline.for compute workloads.
        /// @param pipelineDesc Compute pipeline description.
        /// @return 
        [[nodiscard]] virtual GPUPipeline* CreateComputePipeline(GPUComputePipelineDesc* pipelineDesc) = 0;

        /// @brief Destroy a GPUPipeline.
        /// @param pipeline 
        virtual void DestroyPipeline(GPUPipeline* pipeline) = 0;

        /// @brief Create a GPUCommandBuffer.
        /// @param queue Queue type to create the command buffer for.
        /// @return A new GPUCommandBuffer, or nullptr on failure.
        [[nodiscard]] virtual IGPUCommandBuffer* CreateCommandBuffer(GPUQueueType queue) = 0;

        /// @brief Destroy a GPUCommandBuffer.
        /// @param commandBuffer 
        virtual void DestroyCommandBuffer(IGPUCommandBuffer* commandBuffer) = 0;

        /// @brief Submit work to a GPU queue.
        /// @param queue Queue type to submit work to.
        /// @param commandBuffers Array of command buffers to submit.
        /// @param commandBufferCount Number of command buffers in command buffer array.
        /// @return A GraphicsDeviceResult indicating submit status.
        [[nodiscard]] virtual GraphicsDeviceResult Submit(GPUQueueType queue, IGPUCommandBuffer** commandBuffers, uint32 commandBufferCount) = 0;

        /// @brief Map a buffer region to a host-side pointer.
        /// @param buffer Buffer to map. This buffer must be host-accessible (i.e. created using GPUHeapType_Default or GPUHeapType_Readback).
        /// @param offset Offset in bytes from the start of the buffer.
        /// @param size Size of the buffer region to map in bytes.
        /// @param outData Output data pointer, points to the start of the mapped buffer region.
        /// @return A GraphicsDeviceResult indicating map status.
        [[nodiscard]] virtual GraphicsDeviceResult MapBuffer(GPUBuffer* buffer, int64 offset, uint64 size, void** outData) const = 0;

        /// @brief Unmap a buffer from memory.
        /// @param buffer Buffer to unmap.
        virtual void UnmapBuffer(GPUBuffer* buffer) const = 0;

        /// @brief Wait for the graphics device to be idle (i.e. all submitted work has finished).
        virtual void WaitIdle() = 0;
    };

    /// @brief Create an RHI graphics device.
    /// @param graphicsDeviceDesc Graphics device description.
    /// @return A newly create graphics device, or nullptr on failure.
    [[nodiscard]] BONSAI_RHI_EXPORT IGraphicsDevice* BONSAI_RHI_APICALL CreateGraphicsDevice(GraphicsDeviceDesc* graphicsDeviceDesc);

    /// @brief Destroy an RHI graphics device.
    /// @param graphicsDevice Graphics device to destroy.
    BONSAI_RHI_EXPORT void BONSAI_RHI_APICALL DestroyGraphicsDevice(IGraphicsDevice* graphicsDevice);

    /// @brief Check if a bit flag is set in a bit set.
    /// @tparam BitSetType Type of the bit set.
    /// @tparam FlagType Type of the bit flag.
    /// @param bitset Bitset to use for test.
    /// @param flag Flag to test.
    /// @return A boolean indicating if the flag is set.
    template<typename BitSetType, typename FlagType>
    constexpr bool BitFlagIsSet(BitSetType bitset, FlagType flag) {
        return (bitset & flag) != 0;
    }
} // namespace RHI

#endif //BONSAI_RHI_HPP
