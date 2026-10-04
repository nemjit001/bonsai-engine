#pragma once
#ifndef BONSAI_ASSET_MANAGER_HPP
#define BONSAI_ASSET_MANAGER_HPP

#include <cstdint>
#include <filesystem>
#include <unordered_map>
#include "AssetImporter.hpp"
#include "AssetType.hpp"

namespace Bonsai
{
    class AssetManager;

    /// @brief The Asset Handle refers to an Asset Type stored in an asset manager.
    /// An asset can be retrieved from the asset manager through its handle.
    /// @tparam AssetType 
    template<typename AssetType>
    class AssetHandle
    {
    public:
        constexpr AssetHandle() = default;
        constexpr AssetHandle(uint32_t id, AssetManager* owner)
            : _id(id), _owner(owner) {}

        /// @brief Get the underlying asset.
        /// @return The asset pointer this handle points to.
        std::shared_ptr<AssetType> Get() const;

        /// @brief Get the asset ID.
        /// @return The asset ID stored in the handle.
        uint32_t const& GetAssetId() const;

        /// @brief Check if the asset handle is valid.
        /// @return 
        bool IsValid() const;

        bool operator==(AssetHandle<AssetType> const& rhs) const noexcept { return this->_id == rhs._id; }
        bool operator!=(AssetHandle<AssetType> const& rhs) const noexcept { return !(*this == rhs); }

        /// @brief Invalid asset ID.
        static constexpr uint32_t INVALID_ASSET_ID = (~0u);

    private:
        uint32_t _id = INVALID_ASSET_ID;
        AssetManager* _owner = nullptr;
    };

    /// @brief The Asset Manager handles the loading of assets in a centralized place.
    class AssetManager
    {
    public:
        /// @brief Load an asset from disk.
        /// @tparam AssetType Type of the asset to load.
        /// @param path Path to load asset from.
        /// @return An asset handle that refers to the asset type.
        template<typename AssetType>
        AssetHandle<AssetType> Load(std::filesystem::path const& path);

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

        /// @brief Check if an asset exists in the asset manager.
        /// @tparam AssetType Type of the asset.
        /// @param handle Handle to the asset to test.
        /// @return `true` if the asset exists.
        template<typename AssetType>
        bool AssetExists(AssetHandle<AssetType> const& handle) const;

    private:
        /// @brief Allocate an asset handle for an asset pointer.
        /// @tparam AssetType Asset type of the handle.
        /// @param asset Asset pointer to store in new handle.
        /// @return An asset handle that refers to the asset pointer.
        template<typename AssetType>
        AssetHandle<AssetType> AllocateHandle(std::shared_ptr<IAssetType> const& asset);

    private:
        std::unordered_map<uint32_t, std::shared_ptr<IAssetType>> _assets;
    };

    template<typename AssetType>
    std::shared_ptr<AssetType> AssetHandle<AssetType>::Get() const
    {
        return _owner->GetAsset<AssetType>(*this);
    }

    template<typename AssetType>
    uint32_t const& AssetHandle<AssetType>::GetAssetId() const
    {
        return _id;
    }

    template<typename AssetType>
    bool AssetHandle<AssetType>::IsValid() const
    {
        return _id != INVALID_ASSET_ID
            && _owner != nullptr
            && _owner->AssetExists(*this);
    }

    template<typename AssetType>
    AssetHandle<AssetType> AssetManager::Load(std::filesystem::path const& path)
    {
        static_assert(std::is_base_of_v<IAssetType, AssetType>, "AssetType must inherit from the IAssetType interface!");

        // Import asset using templated asset importer.
        std::shared_ptr<AssetType> assetPtr = AssetImporter<AssetType>::Import(path);
        if (!assetPtr) {
            return {};
        }

        return AllocateHandle<AssetType>(assetPtr);
    }

    template<typename AssetType, typename... Args>
    AssetHandle<AssetType> AssetManager::Create(Args&&... args)
    {
        static_assert(std::is_base_of_v<IAssetType, AssetType>, "AssetType must inherit from the IAssetType interface!");

        // Create a new asset pointer and allocate a handle.
        std::shared_ptr<IAssetType> const assetPtr = std::make_shared<AssetType>(std::forward<Args>(args)...);
        return AllocateHandle<AssetType>(assetPtr);
    }

    template<typename AssetType>
    void AssetManager::Remove(AssetHandle<AssetType> const& handle)
    {
        static_assert(std::is_base_of_v<IAssetType, AssetType>, "AssetType must inherit from the IAssetType interface!");

        // Cannot remove asset that does not exist
        if (!AssetExists(handle)) {
            return;
        }

        // Reset pointer to asset
        std::shared_ptr<IAssetType>& asset = _assets.at(handle.GetAssetId());
        asset.reset();
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
        std::shared_ptr<IAssetType> const asset = _assets.at(handle.GetAssetId());
        return std::dynamic_pointer_cast<AssetType>(asset);
    }

    template<typename AssetType>
    bool AssetManager::AssetExists(AssetHandle<AssetType> const& handle) const
    {
        return _assets.contains(handle.GetAssetId()) && _assets.at(handle.GetAssetId()) != nullptr;
    }

    template<typename AssetType>
    AssetHandle<AssetType> AssetManager::AllocateHandle(std::shared_ptr<IAssetType> const& asset)
    {
        static_assert(std::is_base_of_v<IAssetType, AssetType>, "AssetType must inherit from the IAssetType interface!");

        // Take current size of the map as asset ID
        uint32_t const assetId = static_cast<uint32_t>(_assets.size());

        // Insert the asset at the allocated ID
        _assets.insert(std::make_pair(assetId, asset));

        // Return new asset handle
        return AssetHandle<AssetType>(assetId, this);
    }
} // namespace Bonsai

#endif //BONSAI_ASSET_MANAGER_HPP
