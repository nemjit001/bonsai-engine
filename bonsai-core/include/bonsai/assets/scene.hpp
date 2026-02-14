#pragma once
#ifndef BONSAI_CORE_ASSETS_SCENE_HPP
#define BONSAI_CORE_ASSETS_SCENE_HPP

#include <string>
#include <cereal/cereal.hpp>
#include "asset_loader.hpp"

namespace bonsai::assets
{
    /// @brief The scene class is used to store scene assets. This is NOT the runtime representation
    /// of a scene, only the entity and component data required to initialize the scene in memory.
    class Scene
    {
    public:
        template<class Archive>
        void serialize(Archive& archive)
        {
            archive(CEREAL_NVP(name));
        }

    public:
        std::string name = "Bonsai Scene";
        // TODO(nemjit001): Have a list of entities and their components in asset
    };

    template<>
    std::optional<Scene> AssetLoader<Scene>::load(core::Path const& path);
} // namespace bonsai::assets

#endif //BONSAI_CORE_ASSETS_SCENE_HPP
