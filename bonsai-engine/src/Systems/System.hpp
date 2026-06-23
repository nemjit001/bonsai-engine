#pragma once
#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <BonsaiBase.hpp>
#include <Containers/DynamicArray.hpp>

class World;

/// @brief An ECS System provides a base interface for the implementation of ECS systems.
class System
{
public:
    System() = default;
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

/// @brief The ECS SystemRegistry contains the ECS systems and provides scheduling and update behaviour for those systems.
class SystemRegistry
{
public:
    template<typename SystemType>
    void Register();

    /// @brief Tick all ECS systems in the defined update order.
    /// @param deltaTime 
    void Tick(float deltaTime);

private:
    DynamicArray<std::unique_ptr<System>> _systems;
};

template<typename SystemType>
void SystemRegistry::Register()
{
    BONSAI_STATIC_ASSERT(std::is_base_of_v<System, SystemType>);
    _systems.emplace_back(std::make_unique<SystemType>());
}

#endif //SYSTEM_HPP
