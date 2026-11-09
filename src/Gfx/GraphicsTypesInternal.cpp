#include "GraphicsTypesInternal.hpp"

namespace Bonsai
{
    void FrameContext::AddResourceRef(GraphicsResource* resource)
    {
        // Add a reference to the resource and store in frame resources
        resource->AddRef();
        _resourceRefs.push_back(resource);
    }

    void FrameContext::ReleaseResourceRefs()
    {
        // Release all resource refs
        for (auto const& resource : _resourceRefs)
        {
            if (resource != nullptr) {
                resource->ReleaseRef();
            }
        }

        // Clear held references
        _resourceRefs.clear();
    }

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
} // namespace Bonsai
