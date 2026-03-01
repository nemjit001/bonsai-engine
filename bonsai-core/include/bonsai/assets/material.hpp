#pragma once
#ifndef BONSAI_CORE_ASSETS_MATERIAL_HPP
#define BONSAI_CORE_ASSETS_MATERIAL_HPP

#include <string>
#include <cereal/cereal.hpp>
#include "asset_loader.hpp"

namespace bonsai::assets
{
    /// @brief The Material class is used to represent a basic PBR material.
    class Material
    {
    public:
        template<class Archive>
        void serialize(Archive& archive)
        {
            archive(
                CEREAL_NVP(name),
                CEREAL_NVP(albedo),
                CEREAL_NVP(metallic),
                CEREAL_NVP(roughness)
            );
        }

    public:
        std::string name    = "Bonsai Material";
        float albedo[3]     = { 0.5F, 0.5F, 0.5F };
        float metallic      = 0.0F;
        float roughness     = 0.5F;
    };

    template<>
    std::optional<Material> AssetLoader<Material>::load(core::Path const& path);
} // namespace bonsai::assets

#endif //BONSAI_CORE_ASSETS_MATERIAL_HPP
