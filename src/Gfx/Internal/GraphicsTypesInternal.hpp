#pragma once
#ifndef BONSAI_GRAPHICS_TYPES_INTERNAL_HPP
#define BONSAI_GRAPHICS_TYPES_INTERNAL_HPP

#include <array>
#include <atomic>
#include "Gfx/GraphicsTypes.hpp"
#include "Gfx/Internal/GraphicsCommandBufferInternal.hpp"

namespace Bonsai
{
    /// @brief The GraphicsResource base class implements lifetime management through refcounting.
    /// The class cannot be instantiated on the stack, and heap allocations must be cleaned up through releasing held references.
    class GraphicsResource
    {
    public:
        GraphicsResource() = default;

    protected:
        virtual ~GraphicsResource() = default;

    public:
        GraphicsResource(GraphicsResource const&) = delete;
        GraphicsResource& operator=(GraphicsResource const&) = delete;

    public:
        /// @brief Add a reference to the refcount.
        /// @return The new (stale) reference count.
        virtual uint32_t AddRef() final;

        /// @brief Release a reference from the refcount.
        /// @return The new (stale) reference count.
        virtual uint32_t ReleaseRef() final;

        /// @brief Destroy all held resources immediately.
        /// This is called internally by ReleaseRef when the refcount reaches 0.
        /// WARNING: Calling this function may cause still ref'd resources to become invalidated.
        virtual void Destroy() final;

        /// @brief Get the refcount for this resource.
        /// @return The current (stale) refcount.
        virtual uint32_t GetRefCount() const final;

    private:
        std::atomic<uint32_t> _refCount = 1; //< Initialized to 1 since on creation a reference exists.
    };

    /// @brief The GraphicsBuffer is a graphics resource containing buffer data.
    class GraphicsBuffer : public GraphicsResource
    {
    public:
        /// @brief Set the mapped memory pointer for this buffer.
        /// @param mappedMemory Mapped memory pointer.
        virtual void SetMappedMemory(void* mappedMemory) final;

        /// @brief Get the mapped memory pointer for this buffer.
        /// @return The mapped memory pointer, may be nullptr if buffer is not host visible.
        virtual void* GetMappedMemory() final { return _mappedMemory; }

    private:
        void* _mappedMemory = nullptr;
    };

    /// @brief The GraphicsTexture is a graphics resource containing texture data.
    class GraphicsTexture : public GraphicsResource
    {
        //
    };

    /// @brief The GraphicsTextureView is a graphics resource containig texture view data.
    /// Also tracks the graphics texture that was used to create this view.
    class GraphicsTextureView : public GraphicsResource
    {
    public:
        GraphicsTextureView(GraphicsTexture* parentTexture);

    protected:
        ~GraphicsTextureView() override;

    public:
        /// @brief Get the parent texture for this texture view.
        /// @return The stopred parent texture.
        virtual GraphicsTexture* GetParentTexture() final { return _parentTexture; }

    private:
        GraphicsTexture* _parentTexture = nullptr; //< Parent texture that must remain valid while this view exists.
    };

    /// @brief The GraphicsSampler is a graphics resource containing sampler data.
    class GraphicsSampler : public GraphicsResource
    {
        //
    };

    /// @brief The GraphicsSwapchain is a graphics resource containing swapchain data.
    class GraphicsSwapchain : public GraphicsResource
    {
    public:
        GraphicsSwapchain() = default;

    protected:
        ~GraphicsSwapchain();

    public:
        /// @brief Set the swapchain extent.
        /// @param extent New extent.
        virtual void SetExtent(Extent2D const& extent) { _extent = extent; }

        /// @brief Get the swapchain extent.
        /// @return The current swapchain extent.
        virtual Extent2D GetExtent() const { return _extent; }

        /// @brief Get the backbuffer texture view for the swapchain.
        /// @return The graphics texture view.
        virtual GraphicsTexture* GetBackbuffer() final { return _backbuffer; }

        /// @brief Get the backbuffer texture view for the swapchain.
        /// @return The graphics texture view.
        virtual GraphicsTextureView* GetBackbufferView() final { return _backbufferView; }

        /// @brief Set the backbuffer texture and view for the swapchain.
        /// @param texture Texture to set.
        /// @param textureView Texture view to set.
        virtual void SetBackbufferData(GraphicsTexture* texture, GraphicsTextureView* textureView) final { _backbuffer = texture; _backbufferView = textureView; }

    protected:
        Extent2D _extent = { 0, 0 };
        GraphicsTexture* _backbuffer = nullptr;
        GraphicsTextureView* _backbufferView = nullptr;
    };

    /// @brief The FrameContext is used to store per-frame resources.
    class FrameContext
    {
    public:
        FrameContext() = default;
        virtual ~FrameContext() = default;

        FrameContext(FrameContext const&) = delete;
        FrameContext& operator=(FrameContext const&) = delete;

        FrameContext(FrameContext&&) = default;
        FrameContext& operator=(FrameContext&&) = default;

        /// @brief Reset the frame context in preparation for rendering.
        void Reset();

        /// @brief Begin a new command buffer in the frame.
        /// @return The command buffer.
        GraphicsCommandBuffer* BeginNextGraphicsCommandBuffer();

        /// @brief Get number of recorded command buffers.
        /// @return The number of recorded command buffers.
        size_t GetRecordedCommandBufferCount() const { return _nextCommandBuffer; }
        
        /// @brief Get the recorded command buffer at the given index.
        /// @param index Index of the recorded command buffer.
        /// @return The recorded command buffer.
        GraphicsCommandBufferImpl const* GetRecordedCommandBuffer(size_t index) const { return &_commandBuffers[index]; }

    private:
        uint32_t _nextCommandBuffer = 0;
        std::array<GraphicsCommandBufferImpl, Graphics::MaxGraphicsCommandBuffers> _commandBuffers;
    };

    /// @brief The TransferBatch is used to store a batch of resource transfers.
    class TransferBatch
    {
    public:
        TransferBatch() = default;
        virtual ~TransferBatch() = default;

        TransferBatch(TransferBatch const&) = delete;
        TransferBatch& operator=(TransferBatch const&) = delete;

        TransferBatch(TransferBatch&&) = default;
        TransferBatch& operator=(TransferBatch&&) = default;

        /// @brief Reset the transfer batch in preparation for command recording.
        void Reset();

        /// @brief Begin a new command buffer in the transfer batch.
        /// @return The command buffer.
        GraphicsCommandBuffer* BeginNextTransferCommandBuffer();

        /// @brief Get number of recorded command buffers.
        /// @return The number of recorded command buffers.
        size_t GetRecordedCommandBufferCount() const { return _nextCommandBuffer; }

        /// @brief Get the recorded command buffer at the given index.
        /// @param index Index of the recorded command buffer.
        /// @return The recorded command buffer.
        GraphicsCommandBufferImpl const* GetRecordedCommandBuffer(size_t index) const { return &_commandBuffers[index]; }

    private:
        uint32_t _nextCommandBuffer = 0;
        std::array<GraphicsCommandBufferImpl, Graphics::MaxGraphicsCommandBuffers> _commandBuffers;
    };
} // namespace Bonsai

#endif //BONSAI_GRAPHICS_TYPES_INTERNAL_HPP
