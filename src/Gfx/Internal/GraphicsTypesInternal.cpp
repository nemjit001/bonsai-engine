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

        GraphicsTextureViewType GetGraphicsTextureViewType(GraphicsTextureType const& textureType, size_t depthOrLayers)
        {
            if (depthOrLayers <= 1)
            {
                switch (textureType)
                {
                case GraphicsTextureType_1D:
                    return GraphicsTextureViewType_1D;
                case GraphicsTextureType_2D:
                    return GraphicsTextureViewType_2D;
                case GraphicsTextureType_3D:
                    return GraphicsTextureViewType_3D;
                default:
                    break;
                }
            }
            else
            {
                switch (textureType)
                {
                case GraphicsTextureType_1D:
                    return GraphicsTextureViewType_1DArray;
                case GraphicsTextureType_2D:
                    return GraphicsTextureViewType_2DArray;                    
                case GraphicsTextureType_3D:
                    return GraphicsTextureViewType_3D; // 3D texture arrays are not supported
                default:
                    break;
                }
            }

            return GraphicsTextureViewType_1D;
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
