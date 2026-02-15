#include "bonsai/assets/scene.hpp"

#include <fstream>
#include <cereal/archives/json.hpp>

namespace bonsai::assets
{
    Scene::Scene(Scene&& other) noexcept
        :
        name(std::exchange(other.name, std::string{})),
        registry(std::exchange(other.registry, entt::registry{}))
    {
        //
    }

    Scene& Scene::operator=(Scene&& other) noexcept
    {
        std::swap(name, other.name);
        std::swap(registry, other.registry);
        return *this;
    }

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
