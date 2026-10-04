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
        /// @brief Get the ECS registry for the sccene.
        /// @return 
        entt::registry& GetRegistry() { return _registry; }

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
