#pragma once
#ifndef BONSAI_CORE_ASSETS_ASSET_LOADER_HPP
#define BONSAI_CORE_ASSETS_ASSET_LOADER_HPP

#include <optional>

#include "bonsai/core/filesystem.hpp"

namespace bonsai::assets
{
    /// @brief Asset loader template. Specialize this to provide loading functionality for asset types.
    /// @tparam AssetType 
    template<class AssetType>
    struct AssetLoader
    {
        /// @brief Load an asset from disk.
        /// @param path Path to use for asset loading.
        /// @return 
        static std::optional<AssetType> load(core::Path const& path);
    };
} // namespace bonsai::assets


#endif //BONSAI_CORE_ASSETS_ASSET_LOADER_HPP
