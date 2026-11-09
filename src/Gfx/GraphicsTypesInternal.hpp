#pragma once
#ifndef BONSAI_GRAPHICS_TYPES_INTERNAL_HPP
#define BONSAI_GRAPHICS_TYPES_INTERNAL_HPP

#include <atomic>
#include <vector>
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
        //
    };

    /// @brief The GraphicsTexture is a graphics resource containing texture data.
    class GraphicsTexture : public GraphicsResource
    {
        //
    };

    /// @brief The GraphicsTextureView is a graphics resource containnig texture view data.
    /// Also tracks the graphics texture that was used to create this view.
    class GraphicsTextureView : public GraphicsResource
    {
    public:
        GraphicsTextureView(GraphicsTexture* parentTexture);

    protected:
        ~GraphicsTextureView() override;

    private:
        GraphicsTexture* _parentTexture = nullptr; //< Parent texture that must remain valid while this view exists.
    };

    /// @brief The GraphicsSwapchain is a graphics resource containing swapchain data.
    class GraphicsSwapchain : public GraphicsResource
    {
        //
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

        /// @brief Reference a graphics resource in this frame.
        /// @param resource Resource to reference.
        void AddResourceRef(GraphicsResource* resource);

        /// @brief Release all resource references for this frame.
        void ReleaseResourceRefs();

    private:
        std::vector<GraphicsResource*> _resourceRefs;
    };
} // namespace Bonsai

#endif //BONSAI_GRAPHICS_TYPES_INTERNAL_HPP
