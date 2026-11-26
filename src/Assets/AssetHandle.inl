#pragma once
#ifndef BONSAI_ASSET_HANDLE_INL
#define BONSAI_ASSET_HANDLE_INL

#include "Assets/AssetHandle.hpp"
#include "Assets/AssetManager.hpp"

namespace Bonsai
{
    template<typename AssetType>
    std::shared_ptr<AssetType> AssetHandle<AssetType>::Get() const
    {
        return _owner->GetAsset<AssetType>(*this);
    }


    template<typename AssetType>
    std::optional<Filesystem::Path> AssetHandle<AssetType>::GetAssetPath() const
    {
        return _owner->GetAssetPath(*this);
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
} // namespace Bonsai

#endif //BONSAI_ASSET_HANDLE_INL
