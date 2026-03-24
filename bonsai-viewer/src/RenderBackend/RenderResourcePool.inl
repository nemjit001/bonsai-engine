#pragma once
#ifndef RENDER_RESOURCE_POOL_INL
#define RENDER_RESOURCE_POOL_INL

#include "Assert.hpp"
#include "RenderResourcePool.hpp"

template<class RenderType>
constexpr TRenderHandle<RenderType> TRenderResourcePool<RenderType>::CreateInvalidHandle()
{
    return {
        0xFFFFFF, /* Resource ID */
        0xFF, /* Resource Version */
    };
}

template<class RenderType>
TRenderHandle<RenderType> TRenderResourcePool<RenderType>::Allocate(RenderType NewResource)
{
    BONSAI_ASSERT(Resources.size() == ResourceVersions.size());

    // Try to allocate new resource from freelist
    if (!FreeList.empty())
    {
        uint32_t const ResourceId = FreeList.back();
        FreeList.pop_back();

        uint32_t const ResourceVersion = ResourceVersions[ResourceId];
        Resources[ResourceId] = NewResource;
        return { ResourceId, ResourceVersion };
    }

    // Allocate new resource entry in pool
    uint32_t const ResourceId = static_cast<uint32_t>(Resources.size());
    Resources.push_back(NewResource);
    ResourceVersions.push_back(0);

    return { ResourceId, 0 };
}

template<class RenderType>
void TRenderResourcePool<RenderType>::Free(TRenderHandle<RenderType> Handle)
{
    if (Handle.ResourceId >= Resources.size()) {
        return;
    }

    if (Handle.ResourceVersion != ResourceVersions[Handle.ResourceId]) {
        return;
    }

    ResourceVersions[Handle.ResourceId] += 1;
    FreeList.push_back(Handle.ResourceId);
}

template<class RenderType>
bool TRenderResourcePool<RenderType>::GetResource(TRenderHandle<RenderType> Handle, RenderType& OutResource) const
{
    if (!IsValidResource(Handle)) {
        return false;
    }

    OutResource = Resources[Handle.ResourceId];
    return true;
}

template<class RenderType>
bool TRenderResourcePool<RenderType>::IsValidResource(TRenderHandle<RenderType> Handle) const
{
    BONSAI_ASSERT(Resources.size() == ResourceVersions.size());
    if (Handle.ResourceId >= Resources.size()) {
        return false;
    }

    if (Handle.ResourceVersion != ResourceVersions[Handle.ResourceId]) {
        return false;
    }

    return true;
}

#endif //RENDER_RESOURCE_POOL_INL
