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

    static constexpr float LOD_CLAMP_NONE = 1'000.0F; //< Disable LOD clamp in GPUSamplers.

    /// @brief Enumeration of GraphicsDevice result codes.
    enum GraphicsDeviceResult : uint32
    {
        GraphicsDeviceResult_Ok = 0,
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

    /// @brief Enumeration of GPU texture types.
    enum GPUTextureType : uint32
    {
        GPUTextureType_1D   = 0,
        GPUTextureType_2D   = 1,
        GPUTextureType_3D   = 2,
    };

    /// @brief Enumeration of GPU texture view types.
    enum GPUTextureViewType : uint32
    {
        GPUTextureViewType_1D           = 0,
        GPUTextureViewType_2D           = 1,
        GPUTextureViewType_3D           = 2,
        GPUTextureViewType_Cube         = 3,
        GPUTextureViewType_1DArray      = 4,
        GPUTextureViewType_2DArray      = 5,
        GPUTextureViewType_CubeArray    = 6,
    };

    /// @brief Enumeration of GPU texture formats.
    enum GPUTextureFormat : uint32
    {
        GPUTextureFormat_Unknown    = 0,
    };

    /// @brief Enumeration of GPU filter modes.
    enum GPUFilter : uint32
    {
        GPUFilter_Nearest   = 0,
        GPUFilter_Linear    = 1,
    };

    /// @brief Enumeration of GPU sampler mipmap modes.
    enum GPUSamplerMipmapMode : uint32
    {
        GPUSamplerMipmapMode_Nearest    = 0,
        GPUSamplerMipmapMode_Linear     = 1,
    };

    /// @brief Enumeration of GPU sampler address modes.
    enum GPUSamplerAddressMode : uint32
    {
        GPUSamplerAddressMode_Repeat            = 0,
        GPUSamplerAddressMode_MirroredRepeat    = 1,
        GPUSamplerAddressMode_ClampToEdge       = 2,
        GPUSamplerAddressMode_ClampToBorder     = 3,
    };

    /// @brief Enumeration of GPU comparison functions.
    enum GPUCompareOp : uint32
    {
        GPUCompareOp_Never          = 0,
        GPUCompareOp_Less           = 1,
        GPUCompareOp_Equal          = 2,
        GPUCompareOp_LessOrEqual    = 3,
        GPUCompareOp_Greater        = 4,
        GPUCompareOp_NotEqual       = 5,
        GPUCompareOp_GreaterOrEqual = 6,
        GPUCompareOp_Always         = 7,
    };

    /// @brief Enumeration of GPU blend operations.
    enum GPUBlendOp : uint32
    {
        GPUBlendOp_Add          = 0,
        GPUBlendOp_Subtract     = 1,
        GPUBlendOp_RevSubtract  = 2,
        GPUBlendOp_Min          = 3,
        GPUBlendOp_Max          = 4,
    };

    /// @brief Bitflags for GPU buffer usage.
    enum GPUBufferUsageFlagBits : uint32
    {
        GPUBufferUsage_TransferSrc      = 0x00000001,
        GPUBufferUsage_TransferDst      = 0x00000002,
        GPUBufferUsage_UniformBuffer    = 0x00000010,
        GPUBufferUsage_StorageBuffer    = 0x00000020,
        GPUBufferUsage_IndexBuffer      = 0x00000040,
        GPUBufferUsage_VertexBuffer     = 0x00000080,
        GPUBufferUsage_IndirectBuffer   = 0x00000100,
    };
    typedef uint32 GPUBufferUsageFlags;

    /// @brief Bitflags for GPU texture usage.
    enum GPUTextureUsageFlagBits : uint32
    {
        GPUTextureUsage_TransferSrc         = 0x00000001,
        GPUTextureUsage_TransferDest        = 0x00000002,
        GPUTextureUsage_Sampled             = 0x00000004,
        GPUTextureUsage_Storage             = 0x00000008,
        GPUTextureUsage_RenderTarget        = 0x00000010,
        GPUTextureUsage_DepthStencilTarget  = 0x00000020,
    };
    typedef uint32 GPUTextureUsageFlags;

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

    /// @brief A GPUBufferDesc is used to create GPUBuffers.
    struct GPUBufferDesc
    {
        GPUHeapType heapType;
        uint32 size;
    };

    /// @brief A GPUTextureDesc is used to create GPUTextures.
    struct GPUTextureDesc
    {
        GPUTextureType textureType;
        Extent3D dimensions;
        GPUTextureFormat format = GPUTextureFormat_Unknown;
        uint32 mipLevels = 1;
        uint32 arrayLayers = 1;
        uint32 sampleCount = 1;
        GPUTextureUsageFlags usage = 0;
    };

    /// @brief A GPUSamplerDesc is used to create GPUSamplers.
    struct GPUSamplerDesc
    {
        GPUFilter magFilter = GPUFilter_Nearest;
        GPUFilter minFilter = GPUFilter_Nearest;
        GPUSamplerMipmapMode mipmapMode = GPUSamplerMipmapMode_Nearest;
        GPUSamplerAddressMode addressModeU = GPUSamplerAddressMode_Repeat;
        GPUSamplerAddressMode addressModeV = GPUSamplerAddressMode_Repeat;
        GPUSamplerAddressMode addressModeW = GPUSamplerAddressMode_Repeat;
        float mipLODBias = 0.0F;
        uint32 maxAnisotropy = 0;
        GPUCompareOp compareOp = GPUCompareOp_Never;
        float minLOD = 0.0F;
        float maxLOD = LOD_CLAMP_NONE;
    };

    /// @brief A GPUComputePipelineDesc is used to create compute GPUPipelines.
    struct GPUComputePipelineDesc
    {
        //
    };

    /// @brief A GPUGraphicsPipelineDesc is used to create raster GPUPipelines.
    struct GPUGraphicsPipelineDesc
    {
        //
    };

    /// @brief A GraphicsDeviceDesc is used to initialize a GraphicsDevice.
    struct GraphicsDeviceDesc
    {
        uint32 framesInFlight = 2; //< Number of frames to process simultaneously.
    };

    /// @brief The GPUCommandBuffer interface implements command recording on top of low-level graphics command buffers.
    class BONSAI_RHI_EXPORT IGPUCommandBuffer
    {
    public:
        virtual ~IGPUCommandBuffer() = default;
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
        /// @return A new GPUCommandBuffer, or nullptr on failure.
        [[nodiscard]] virtual IGPUCommandBuffer* CreateCommandBuffer() = 0;

        /// @brief Destroy a GPUCommandBuffer.
        /// @param commandBuffer 
        virtual void DestroyCommandBuffer(IGPUCommandBuffer* commandBuffer) = 0;

        /// @brief Submit work to a GPU queue.
        /// @param queue Queue type to submit work to.
        /// @param commandBuffers Array of command buffers to submit.
        /// @param commandBufferCount Number of command buffers in command buffer array.
        /// @return A GraphicsDeviceResult indicating submit status.
        [[nodiscard]] virtual GraphicsDeviceResult Submit(GPUQueueType queue, IGPUCommandBuffer** commandBuffers, uint32 commandBufferCount) = 0;
    };

    /// @brief Create an RHI graphics device.
    /// @param graphicsDeviceDesc Graphics device description.
    /// @return A newly create graphics device, or nullptr on failure.
    [[nodiscard]] BONSAI_RHI_EXPORT IGraphicsDevice* BONSAI_RHI_APICALL CreateGraphicsDevice(GraphicsDeviceDesc* graphicsDeviceDesc);

    /// @brief Destroy an RHI graphics device.
    /// @param graphicsDevice Graphics device to destroy.
    BONSAI_RHI_EXPORT void BONSAI_RHI_APICALL DestroyGraphicsDevice(IGraphicsDevice* graphicsDevice);
} // namespace RHI

#endif //BONSAI_RHI_HPP
