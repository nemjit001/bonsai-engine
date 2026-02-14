#include "bonsai/assets/scene.hpp"

#include <fstream>
#include <cereal/archives/json.hpp>

namespace bonsai::assets
{
    template<>
    std::optional<Scene> AssetLoader<Scene>::load(core::Path const& path)
    {
        std::ifstream file(path);
        if (!file) {
            return {};
        }

        cereal::JSONInputArchive archive(file);
        Scene scene{};
        archive(CEREAL_NVP(scene));
        return scene;
    }
} // namespace bonsai::assets
