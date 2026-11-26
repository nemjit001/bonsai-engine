#pragma once
#ifndef BONSAI_ASSET_MANAGER_HPP
#define BONSAI_ASSET_MANAGER_HPP

#include <cstdint>
#include <optional>
#include <memory>
#include <unordered_map>
#include <Core/Filesystem.hpp>
#include "Assets/AssetType.hpp"

namespace Bonsai
{
    template<typename AssetType>
    class AssetHandle;

    /// @brief The Asset Manager handles the loading of assets in a centralized place.
    class AssetManager
    {
    public:
        /// @brief Load an asset from disk.
        /// @tparam AssetType Type of the asset to load.
        /// @param path Path to load asset from.
        /// @return An asset handle that refers to the asset type.
        template<typename AssetType>
        AssetHandle<AssetType> Load(Filesystem::Path const& path);

        /// @brief Create an asset in the asset manager.
        /// @tparam AssetType Type of the asset to create.
        /// @tparam Args Asset ctor arg types.
        /// @param args Asset ctor args.
        /// @return An asset handle that refers to the asset type.
        template<typename AssetType, typename... Args>
        AssetHandle<AssetType> Create(Args&&... args);

        /// @brief Remove an asset from the asset manager.
        /// @tparam AssetType Type of the asset to remove.
        /// @param handle Asset handle to remove from the store.
        template<typename AssetType>
        void Remove(AssetHandle<AssetType> const& handle);

        /// @brief Get an asset from the asset manager.
        /// @tparam AssetType Type of the asset to retrieve.
        /// @param handle Handle to the asset to retrieve.
        /// @return The pointer to the retrieved asset.
        template<typename AssetType>
        std::shared_ptr<AssetType> GetAsset(AssetHandle<AssetType> const& handle) const;

        /// @brief Get an asset path from the asset manager. Depending on how the asset was created this may be empty.
        /// @tparam AssetType Type of the asset to retrieve the path from.
        /// @param handle Handle to the asset for which to retrieve the path.
        /// @return An optional asset path.
        template<typename AssetType>
        std::optional<Filesystem::Path> GetAssetPath(AssetHandle<AssetType> const& handle) const;

        /// @brief Check if an asset exists in the asset manager.
        /// @tparam AssetType Type of the asset.
        /// @param handle Handle to the asset to test.
        /// @return `true` if the asset exists.
        template<typename AssetType>
        bool AssetExists(AssetHandle<AssetType> const& handle) const;

    private:
        /// @brief Entry into the asset manager.
        struct AssetManagerEntry
        {
            std::shared_ptr<IAssetType> asset;
            std::optional<Filesystem::Path> assetPath;
        };

        /// @brief Create an asset handle for an asset pointer.
        /// @tparam AssetType Asset type of the handle.
        /// @param asset Asset pointer to store in new handle.
        /// @return An asset handle that refers to the asset pointer.
        template<typename AssetType>
        AssetHandle<AssetType> CreateAssetHandle(std::shared_ptr<IAssetType> const& asset, std::optional<Filesystem::Path> const& assetPath);

    private:
        std::unordered_map<Filesystem::Path, uint32_t> _assetPaths;
        std::unordered_map<uint32_t, AssetManagerEntry> _assets;
    };
} // namespace Bonsai

namespace std
{
    /// @brief Specialization of AssetHandle hash type.
    template<typename AssetType>
    struct hash<Bonsai::AssetHandle<AssetType>>
    {
    public:
        size_t operator()(Bonsai::AssetHandle<AssetType> const& key) const
        {
            // Simply hashing the asset ID should be enough
            return hash<uint32_t>()(key.GetAssetId());
        }
    };
} // namespace std


#include "AssetManager.inl"
#endif //BONSAI_ASSET_MANAGER_HPP
