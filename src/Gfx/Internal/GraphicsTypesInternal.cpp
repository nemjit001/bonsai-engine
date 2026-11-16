#include "GraphicsTypesInternal.hpp"

#include <cassert>

namespace Bonsai
{
    uint32_t GraphicsResource::AddRef()
    {
        // Add a ref to the refcount, memory order acquire release is used to ensure other threads see accurate refcounts.
        _refCount.fetch_add(1, std::memory_order_acq_rel);
        return _refCount;
    }

    uint32_t GraphicsResource::ReleaseRef()
    {
        // Check if refcount reaches 0 (previous value was 1)
        if (_refCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
        {
            // Destroy the resource
            Destroy();
            return 0;
        }

        return _refCount;
    }

    void GraphicsResource::Destroy()
    {
        // This is safe since the dtor is protected, meaning only heap allocations of this resource type exist.
        delete this;
    }

    uint32_t GraphicsResource::GetRefCount() const
    {
        return _refCount;
    }

    void GraphicsBuffer::SetMappedMemory(void* mappedMemory)
    {
        _mappedMemory = mappedMemory;
    }

    GraphicsTextureView::~GraphicsTextureView()
    {
        if (_parentTexture != nullptr) {
            _parentTexture->ReleaseRef();
        }
    }

    void GraphicsTextureView::SetParentTexture(GraphicsTexture* parentTexture)
    {
        if (_parentTexture != nullptr) {
            _parentTexture->ReleaseRef();
        }

        _parentTexture = parentTexture;
        _parentTexture->AddRef();
    }

    GraphicsSwapchain::~GraphicsSwapchain()
    {
        if (_backbufferView != nullptr) {
            _backbufferView->ReleaseRef();
        }

        if (_backbuffer != nullptr) {
            _backbuffer->ReleaseRef();
        }
    }

    void FrameContext::Reset()
    {
        _nextCommandBuffer = 0;
    }

    GraphicsCommandBuffer* FrameContext::BeginNextGraphicsCommandBuffer()
    {
        // Check if there is a command buffer available
        if (_nextCommandBuffer >= _commandBuffers.size()) {
            return nullptr;
        }

        // Get next command buffer
        GraphicsCommandBuffer& commandBuffer = _commandBuffers[_nextCommandBuffer];
        _nextCommandBuffer++;

        // Reset command buffer
        commandBuffer.Reset();
        return &commandBuffer;
    }

    void TransferBatch::Reset()
    {
        _nextCommandBuffer = 0;
    }

    GraphicsCommandBuffer* TransferBatch::BeginNextTransferCommandBuffer()
    {
        // Check if there is a command buffer available
        if (_nextCommandBuffer >= _commandBuffers.size()) {
            return nullptr;
        }

        // Get next command buffer
        GraphicsCommandBuffer& commandBuffer = _commandBuffers[_nextCommandBuffer];
        _nextCommandBuffer++;

        // Reset command buffer
        commandBuffer.Reset();
        return &commandBuffer;
    }

    namespace GraphicsHelpers
    {
        bool IsColorFormat(GraphicsFormat const& format)
        {
            return format != GraphicsFormat_Unknown
                && !IsDepthFormat(format)
                && !IsStencilFormat(format);
        }
        
        bool IsDepthFormat(GraphicsFormat const& format)
        {
            return format == GraphicsFormat_D16_UNorm
                || format == GraphicsFormat_D24_UNorm_S8_UInt
                || format == GraphicsFormat_D32_SFloat;
        }

        bool IsStencilFormat(GraphicsFormat const& format)
        {
            return format == GraphicsFormat_D24_UNorm_S8_UInt;
        }

        GraphicsTextureAspectFlags GetGraphicsTextureAspectFlags(GraphicsFormat const& format)
        {
            GraphicsTextureAspectFlags output = 0;
            if (IsColorFormat(format)) {
                output |= GraphicsTextureAspect_Color;
            }
            if (IsDepthFormat(format)) {
                output |= GraphicsTextureAspect_Depth;
            }
            if (IsStencilFormat(format)) {
                output |= GraphicsTextureAspect_Stencil;
            }

            return output;
        }
    } // namespace GraphicsHelpers
} // namespace Bonsai
