#pragma once
#ifndef BONSAI_GRAPHICS_TYPES_INTERNAL_HPP
#define BONSAI_GRAPHICS_TYPES_INTERNAL_HPP

#include <array>
#include <atomic>
#include <vector>
#include "Gfx/GraphicsCommandBuffer.hpp"
#include "Gfx/GraphicsTypes.hpp"

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
        /// @brief Get the description used to create the buffer.
        /// @return The buffer description.
        virtual GraphicsBufferDesc GetDesc() const final { return _desc; }

        /// @brief Set the description used to create the buffer.
        /// @param desc The description to set.
        virtual void SetDesc(GraphicsBufferDesc const& desc) final { _desc = desc; }

        /// @brief Get the mapped memory pointer for this buffer.
        /// @return The mapped memory pointer, may be nullptr if buffer is not host visible.
        virtual void* GetMappedMemory() final { return _mappedMemory; }

        /// @brief Set the mapped memory pointer for this buffer.
        /// @param mappedMemory Mapped memory pointer.
        virtual void SetMappedMemory(void* mappedMemory) final;

    private:
        GraphicsBufferDesc _desc = {};
        void* _mappedMemory = nullptr;
    };

    /// @brief The GraphicsTexture is a graphics resource containing texture data.
    class GraphicsTexture : public GraphicsResource
    {
    public:        
        /// @brief Get the description used to create the texture.
        /// @return The texture description.
        virtual GraphicsTextureDesc GetDesc() const final { return _desc; }
        
        /// @brief Set the description used to create the texture.
        /// @param desc The description to set.
        virtual void SetDesc(GraphicsTextureDesc const& desc) final { _desc = desc; }

    private:
        GraphicsTextureDesc _desc = {};
    };

    /// @brief The GraphicsTextureView is a graphics resource containig texture view data.
    /// Also tracks the graphics texture that was used to create this view.
    class GraphicsTextureView : public GraphicsResource
    {
    protected:
        ~GraphicsTextureView() override;

    public:
        /// @brief Get the description used to create the texture view.
        /// @return The texture view description.
        virtual GraphicsTextureViewDesc GetDesc() const final { return _desc; }
        
        /// @brief Set the description used to create the texture view.
        /// @param desc The description to set.
        virtual void SetDesc(GraphicsTextureViewDesc const& desc) final { _desc = desc; }

        /// @brief Get the parent texture for this texture view.
        /// @return The stored parent texture.
        virtual GraphicsTexture* GetParentTexture() final { return _parentTexture; }

        /// @brief Set the parent texture for this texture view.
        /// @param parentTexture The new parent texture.
        virtual void SetParentTexture(GraphicsTexture* parentTexture);

    private:
        GraphicsTextureViewDesc _desc = {};
        GraphicsTexture* _parentTexture = nullptr; //< Parent texture that must remain valid while this view exists.
    };

    /// @brief The GraphicsSampler is a graphics resource containing sampler data.
    class GraphicsSampler : public GraphicsResource
    {
    public:
        /// @brief Get the description used to create the sampler.
        /// @return The sampler description.
        virtual GraphicsSamplerDesc GetDesc() const final { return _desc; }

        /// @brief Set the description used to create the sampler.
        /// @param desc The description to set.
        virtual void SetDesc(GraphicsSamplerDesc const& desc) final { _desc = desc; }

    private:
        GraphicsSamplerDesc _desc = {};
    };

    /// @brief The GraphicsDescriptorSetLayout is a graphics resource containing descriptor set layout data.
    class GraphicsDescriptorSetLayout : public GraphicsResource
    {
        //
    };

    class GraphicsDescriptorSet : public GraphicsResource
    {
        //
    };

    /// @brief The GraphicsPipelineLayout is a graphics resource containing pipeline layout data.
    class GraphicsPipelineLayout : public GraphicsResource
    {
        //
    };

    /// @brief The GraphicsRenderPipeline is a graphics resource containing render pipeline data.
    class GraphicsRenderPipeline : public GraphicsResource
    {
        //
    };

    /// @brief The GraphicsComputePipeline is a graphics resource containing compute pipeline data.
    class GraphicsComputePipeline : public GraphicsResource
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
        /// @brief Get the swapchain extent.
        /// @return The current swapchain extent.
        virtual Extent2D GetExtent() const { return _extent; }
        
        /// @brief Set the swapchain extent.
        /// @param extent New extent.
        virtual void SetExtent(Extent2D const& extent) { _extent = extent; }

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

    public:
        std::vector<GraphicsCommandBuffer*> commandBuffers;
    };

    /// @brief The TransferBatch is used to store a batch of resource transfers.
    class TransferBatch
    {
    public:
        TransferBatch() = default;
        virtual ~TransferBatch() = default;

    public:
        std::vector<GraphicsCommandBuffer*> commandBuffers;
    };

    namespace GraphicsHelpers
    {
        /// @brief Test if a graphics format is a color format.
        /// @param format Format to test.
        /// @return A boolean indicating a color format.
        bool IsColorFormat(GraphicsFormat const& format);

        /// @brief Test if a graphics format is a depth format.
        /// @param format Format to test.
        /// @return A boolean indicating a depth format.
        bool IsDepthFormat(GraphicsFormat const& format);

        /// @brief Test if a graphics format is a stencil format.
        /// @param format Format to test.
        /// @return A boolean indicating a stencil format.
        bool IsStencilFormat(GraphicsFormat const& format);

        /// @brief Get the texture aspect flags for a graphics format.
        /// @param format Format to get aspect flags for.
        /// @return The texture aspect flags.
        GraphicsTextureAspectFlags GetGraphicsTextureAspectFlags(GraphicsFormat const& format);
    } // namespace GraphicsHelpers
} // namespace Bonsai

#endif //BONSAI_GRAPHICS_TYPES_INTERNAL_HPP
