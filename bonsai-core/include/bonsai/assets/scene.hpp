#pragma once
#ifndef BONSAI_CORE_ASSETS_SCENE_HPP
#define BONSAI_CORE_ASSETS_SCENE_HPP

#include <string>
#include <cereal/cereal.hpp>
#include <entt/entt.hpp>
#include "asset_loader.hpp"

namespace bonsai::assets
{
    /// @brief The scene class is used to store scene assets. This is the initial runtime scene state which gets modified during simulation.
    class Scene
    {
    public:
        Scene() = default;
        ~Scene() = default;

        Scene(Scene const&) = delete;
        Scene& operator=(Scene const&) = delete;

        Scene(Scene&& other) noexcept;
        Scene& operator=(Scene&& other) noexcept;

        template<class Archive>
        void save(Archive& archive) const;

        template<class Archive>
        void load(Archive& archive);

    public:
        std::string name = "Bonsai Scene";
        entt::registry registry; // TODO(nemjit001): Check if we can't maintain a single registry with scenes only getting a reference to the central registry (would make reactive systems easier)
    };

    template<>
    std::optional<Scene> AssetLoader<Scene>::load(core::Path const& path);

    template<class Archive>
    void Scene::save(Archive& archive) const
    {
        archive(CEREAL_NVP(name));
    }

    template<class Archive>
    void Scene::load(Archive& archive)
    {
        archive(CEREAL_NVP(name));
    }
} // namespace bonsai::assets

#endif //BONSAI_CORE_ASSETS_SCENE_HPP
