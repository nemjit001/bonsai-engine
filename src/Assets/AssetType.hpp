#pragma once
#ifndef BONSAI_ASSET_TYPE_HPP
#define BONSAI_ASSET_TYPE_HPP

namespace Bonsai
{
    /// @brief The Asset Type interface must be implemented by assets.
    class IAssetType
    {
    public:
        virtual ~IAssetType() = default;
    };
} // namespace Bonsai

#endif //BONSAI_ASSET_TYPE_HPP
