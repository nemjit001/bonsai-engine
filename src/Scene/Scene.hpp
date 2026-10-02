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

    private:
        entt::registry _registry{};
    };
} // namespace Bonsai

#endif //BONSAI_SCENE_HPP
