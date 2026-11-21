#pragma once
#ifndef BONSAI_ASSET_IMPORTER_HPP
#define BONSAI_ASSET_IMPORTER_HPP

#include <memory>
#include <Core/Filesystem.hpp>
#include "Assets/AssetType.hpp"

namespace Bonsai
{
    /// @brief The Asset Importer static class allows for asset importing using its static import function.
    /// Importing behaviour can be automatically added for asset types by providing a template specialization for the asset importer.
    /// @tparam AssetType Asset type to provide import behaviour for.
    template<typename AssetType>
    class AssetImporter
    {
        static_assert(std::is_base_of_v<IAssetType, AssetType>, "AssetType must inherit from the IAssetType interface!");
        
    public:
        /// @brief Import an asset type using this importer.
        /// @param path Path to the asset to import.
        /// @return A pointer to the imported asset.
        static std::shared_ptr<AssetType> Import(Filesystem::Path const& path);
    };
} // namespace Bonsai

#endif //BONSAI_ASSET_IMPORTER_HPP
