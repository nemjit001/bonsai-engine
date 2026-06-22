#pragma once
#ifndef WORLD_HPP
#define WORLD_HPP

#include <entt/entt.hpp>

/// @brief A World is a collection of entities.
class World
{
public:
    /// @brief Get the ECS registry for this world.
    /// @return 
    entt::registry&         GetRegistry()       { return _registry; }
    entt::registry const&   GetRegistry() const { return _registry; }

private:
    entt::registry _registry;
};

#endif //WORLD_HPP
