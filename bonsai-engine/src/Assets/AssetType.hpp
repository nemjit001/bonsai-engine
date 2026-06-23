#pragma once
#ifndef ASSET_TYPE_HPP
#define ASSET_TYPE_HPP

/// @brief The AssetType interface provides an interface for asset saving/loading.
class IAssetType
{
public:
    virtual ~IAssetType() = default;

    IAssetType(IAssetType const&) = default;
    IAssetType& operator=(IAssetType const&) = default;

    IAssetType(IAssetType&&) = default;
    IAssetType& operator=(IAssetType&&) = default;
};

#endif //ASSET_TYPE_HPP
