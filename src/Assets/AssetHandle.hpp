#pragma once
#ifndef BONSAI_ASSET_HANDLE_HPP
#define BONSAI_ASSET_HANDLE_HPP

#include <cstdint>
#include <memory>
#include <optional>
#include <Core/Filesystem.hpp>
#include "Assets/AssetType.hpp"

namespace Bonsai
{
    class AssetManager;

    /// @brief The Asset Handle refers to an Asset Type stored in an asset manager.
    /// An asset can be retrieved from the asset manager through its handle.
    /// @tparam AssetType 
    template<typename AssetType>
    class AssetHandle
    {
        static_assert(std::is_base_of_v<IAssetType, AssetType>, "AssetType must inherit from the IAssetType interface!");

    public:
        constexpr AssetHandle() = default;
        constexpr AssetHandle(uint32_t id, AssetManager* owner)
            : _id(id), _owner(owner) {}

        /// @brief Get the underlying asset.
        /// @return The asset pointer this handle points to.
        std::shared_ptr<AssetType> Get() const;

        /// @brief Get the asset path. This may be empty if the asset was created at runtime.
        /// @return An optional asset path.
        std::optional<Filesystem::Path> GetAssetPath() const;

        /// @brief Get the asset ID.
        /// @return The asset ID stored in the handle.
        uint32_t const& GetAssetId() const;

        /// @brief Check if the asset handle is valid.
        /// @return 
        bool IsValid() const;

        bool operator==(AssetHandle<AssetType> const& rhs) const noexcept { return this->_id == rhs._id && this->_owner == rhs._owner; }
        bool operator!=(AssetHandle<AssetType> const& rhs) const noexcept { return !(*this == rhs); }

        /// @brief Invalid asset ID.
        static constexpr uint32_t INVALID_ASSET_ID = (~0u);

    private:
        uint32_t _id = INVALID_ASSET_ID;
        AssetManager* _owner = nullptr;
    };
} // namespace Bonsai

#include "AssetHandle.inl"
#endif //BONSAI_ASSET_HANDLE_HPP
