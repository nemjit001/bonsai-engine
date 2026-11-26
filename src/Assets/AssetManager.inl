#pragma once
#ifndef BONSAI_ASSET_MANAGER_INL
#define BONSAI_ASSET_MANAGER_INL

#include "Assets/AssetHandle.hpp"
#include "Assets/AssetManager.hpp"
#include "Assets/AssetImporter.hpp"

namespace Bonsai
{
    template<typename AssetType>
    AssetHandle<AssetType> AssetManager::Load(Filesystem::Path const& path)
    {
        static_assert(std::is_base_of_v<IAssetType, AssetType>, "AssetType must inherit from the IAssetType interface!");

        // Check if asset already exists in the asset manager
        if (_assetPaths.contains(path))
        {
            uint32_t const assetId = _assetPaths.at(path);
            return AssetHandle<AssetType>(assetId, this);
        }

        // Import asset using templated asset importer.
        std::shared_ptr<AssetType> assetPtr = AssetImporter<AssetType>::Import(*this, path);
        if (!assetPtr) {
            return {};
        }

        // Allocate new asset handle and cache asset path
        AssetHandle<AssetType> const handle = CreateAssetHandle<AssetType>(assetPtr, path);
        _assetPaths.insert(std::make_pair(path, handle.GetAssetId()));
        return handle;
    }

    template<typename AssetType, typename... Args>
    AssetHandle<AssetType> AssetManager::Create(Args&&... args)
    {
        static_assert(std::is_base_of_v<IAssetType, AssetType>, "AssetType must inherit from the IAssetType interface!");

        // Create a new asset pointer and allocate a handle.
        std::shared_ptr<IAssetType> const assetPtr = std::make_shared<AssetType>(std::forward<Args>(args)...);
        return CreateAssetHandle<AssetType>(assetPtr, {});
    }

    template<typename AssetType>
    void AssetManager::Remove(AssetHandle<AssetType> const& handle)
    {
        static_assert(std::is_base_of_v<IAssetType, AssetType>, "AssetType must inherit from the IAssetType interface!");

        // Cannot remove asset that does not exist
        if (!AssetExists(handle)) {
            return;
        }

        // Reset asset manager entry
        AssetManagerEntry& entry = _assets.at(handle.GetAssetId());
        entry.asset.reset();
        entry.assetPath.reset();
    }
    
    template<typename AssetType>
    std::shared_ptr<AssetType> AssetManager::GetAsset(AssetHandle<AssetType> const& handle) const
    {
        static_assert(std::is_base_of_v<IAssetType, AssetType>, "AssetType must inherit from the IAssetType interface!");

        // Cannot get an asset that does not exist.
        if (!AssetExists(handle)) {
            return {};
        }

        // Return found asset, cast to base type.
        AssetManagerEntry const& entry = _assets.at(handle.GetAssetId());
        return std::dynamic_pointer_cast<AssetType>(entry.asset);
    }

    template<typename AssetType>
    std::optional<Filesystem::Path> AssetManager::GetAssetPath(AssetHandle<AssetType> const& handle) const
    {
        static_assert(std::is_base_of_v<IAssetType, AssetType>, "AssetType must inherit from the IAssetType interface!");

        // Cannot get an asset that does not exist.
        if (!AssetExists(handle)) {
            return {};
        }

        // Return found asset path
        AssetManagerEntry const& entry = _assets.at(handle.GetAssetId());
        return entry.assetPath;
    }

    template<typename AssetType>
    bool AssetManager::AssetExists(AssetHandle<AssetType> const& handle) const
    {
        auto const iter = _assets.find(handle.GetAssetId());
        if (iter == _assets.end()) {
            return false;
        }

        AssetManagerEntry const& entry = iter->second;
        return entry.asset != nullptr;
    }

    template<typename AssetType>
    AssetHandle<AssetType> AssetManager::CreateAssetHandle(std::shared_ptr<IAssetType> const& asset, std::optional<Filesystem::Path> const& assetPath)
    {
        static_assert(std::is_base_of_v<IAssetType, AssetType>, "AssetType must inherit from the IAssetType interface!");

        // Take current size of the map as asset ID
        uint32_t const assetId = static_cast<uint32_t>(_assets.size());

        // Create asset manager entry
        AssetManagerEntry entry{};
        entry.asset = asset;
        entry.assetPath = assetPath;
        _assets.insert(std::make_pair(assetId, entry));

        // Return new asset handle
        return AssetHandle<AssetType>(assetId, this);
    }
} // namespace Bonsai

#endif //BONSAI_ASSET_MANAGER_INL
