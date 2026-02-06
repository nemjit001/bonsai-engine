#pragma once
#ifndef BONSAI_VIEWER_ASSETS_ASSET_LOADER_HPP
#define BONSAI_VIEWER_ASSETS_ASSET_LOADER_HPP

#include <optional>

#include "core/filesystem.hpp"

namespace bonsai::assets
{
    /// @brief Asset loader template. Specialize this to provide loading functionality for asset types.
    /// @tparam AssetType 
    template<class AssetType>
    struct AssetLoader
    {
        static std::optional<AssetType> load(core::Path const& path);
    };
} // namespace bonsai::assets


#endif //BONSAI_VIEWER_ASSETS_ASSET_LOADER_HPP
