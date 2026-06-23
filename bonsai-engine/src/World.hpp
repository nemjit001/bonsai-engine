#pragma once
#ifndef WORLD_HPP
#define WORLD_HPP

#include <entt/entt.hpp>

/// @brief A World contains an ECS registry with all runtime entities.
class World
{
public:
    /// @brief Get the ECS regsitry for the game world.
    /// @return 
    entt::registry& GetRegistry() { return _registry; }

private:
    entt::registry _registry;
};

#endif //WORLD_HPP
