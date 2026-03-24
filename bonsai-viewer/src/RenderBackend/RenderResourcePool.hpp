#pragma once
#ifndef RENDER_RESOURCE_POOL_HPP
#define RENDER_RESOURCE_POOL_HPP

#include <vector>
#include "RenderHandle.hpp"

/// @brief Typed resource pool for internally tracking renderer resources.
/// @tparam RenderType 
template<class RenderType>
class TRenderResourcePool
{
public:
    /// @brief Create an invalid resource handle.
    /// @return 
    static constexpr TRenderHandle<RenderType> CreateInvalidHandle();

    /// @brief Allocate a new render resource from the resource pool.
    /// @param NewResource
    /// @return 
    TRenderHandle<RenderType> Allocate(RenderType NewResource);

    /// @brief Free a render resource from the resource pool.
    /// @param Handle Resource handle.
    void Free(TRenderHandle<RenderType> Handle);

    /// @brief Get a resource by handle from a resource pool.
    /// @param Handle Resource handle.
    /// @param OutResource Output resource.
    /// @return 
    bool GetResource(TRenderHandle<RenderType> Handle, RenderType& OutResource) const;

    /// @brief Check if a resource with a given handle is valid.
    /// @param Handle
    /// @return 
    bool IsValidResource(TRenderHandle<RenderType> Handle) const;

private:
    std::vector<RenderType> Resources;
    std::vector<uint32_t> ResourceVersions;
    std::vector<uint32_t> FreeList;
};

#include "RenderResourcePool.inl"
#endif //RENDER_RESOURCE_POOL_HPP
