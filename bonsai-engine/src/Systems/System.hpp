#pragma once
#ifndef SYSTEM_HPP
#define SYSTEM_HPP

class World;

/// @brief An ECS System provides a base interface for the implementation of ECS systems.
class System
{
public:
    virtual ~System() = default;

    System(System const&) = default;
    System& operator=(System const&) = default;

    System(System&&) = default;
    System& operator=(System&&) = default;

    /// @brief Run an ECS system, modifying the game world.
    /// @param world 
    /// @param deltaTime 
    virtual void Run([[maybe_unused]] World& world, [[maybe_unused]] float deltaTime) {};
};

#endif //SYSTEM_HPP
