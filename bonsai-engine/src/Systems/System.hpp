#pragma once
#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <BonsaiBase.hpp>

class World;

/// @brief System priority, determines phase of frame updates where the system runs.
enum SystemPriority : int32
{
    SystemPriority_Tick = 0,
};

/// @brief An ECS System interface provides a serializable interface for the implementation of ECS systems.
class ISystem
{
public:
    ISystem() = default;
    virtual ~ISystem() = default;

    ISystem(ISystem const&) = default;
    ISystem& operator=(ISystem const&) = default;

    ISystem(ISystem&&) = default;
    ISystem& operator=(ISystem&&) = default;

    /// @brief Run an ECS system, modifying the game world.
    /// @param world 
    /// @param deltaTime 
    virtual void Run([[maybe_unused]] World& world, [[maybe_unused]] float deltaTime) = 0;
};

#endif //SYSTEM_HPP
