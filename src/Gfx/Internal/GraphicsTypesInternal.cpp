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

    GraphicsTextureView::GraphicsTextureView(GraphicsTexture* parentTexture)
        :
        _parentTexture(parentTexture)
    {
        assert(_parentTexture != nullptr && "Parent texture for texture view was null!");
        _parentTexture->AddRef();
    }

    GraphicsTextureView::~GraphicsTextureView()
    {
        _parentTexture->ReleaseRef();
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
} // namespace Bonsai
