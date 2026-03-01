#include "bonsai/assets/material.hpp"

#include <fstream>
#include <cereal/archives/json.hpp>

namespace bonsai::assets
{
    template<>
    std::optional<Material> AssetLoader<Material>::load(core::Path const& path)
    {
        std::ifstream file(path);
        if (!file) {
            return {};
        }

        cereal::JSONInputArchive archive(file);
        Material material{};
        archive(CEREAL_NVP(material));
        return material;
    }
} // namespace bonsai::assets
