#pragma once
#ifndef BONSAI_SCENE_HPP
#define BONSAI_SCENE_HPP

#include <entt/entt.hpp>

namespace Bonsai
{
    class SystemRegistry;

    /// @brief The Scene contains an ECS registry that holds all entities in the scene.
    class Scene
    {
    public:
        /// @brief Tick the scene using a system registry.
        /// @param systemRegistry System registry to use for scene updates.
        void Tick(SystemRegistry const& systemRegistry);

        /// @brief Clear the scene data, resulting in an empty scene.
        void Clear();

        /// @brief Serialization function for scene data. Used with serialization archives.
        /// @tparam Archive 
        /// @param archive 
        template<typename Archive>
        void serialize(Archive& archive);

    private:
        entt::registry _registry{};
    };

    template<typename Archive>
    void Scene::serialize(Archive& archive)
    {
        // TODO(nemjit001): serialize the entt::registry using an entt::snapshot, figure out how the snapshot archiving works.
        archive();
    }
} // namespace Bonsai

#endif //BONSAI_SCENE_HPP
