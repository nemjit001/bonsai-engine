#include "bonsai/assets/shader.hpp"

#include <fstream>
#include <cereal/archives/json.hpp>

namespace bonsai::assets
{
    template<>
    std::optional<Shader> AssetLoader<Shader>::load(core::Path const& path)
    {
        std::ifstream file(path);
        if (!file)
        {
            return {};
        }

        cereal::JSONInputArchive archive(file);
        Shader shader{};
        archive(CEREAL_NVP(shader));
        return shader;
    }
} // namespace bonsai::assets
