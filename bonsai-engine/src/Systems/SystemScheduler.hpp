#pragma once
#ifndef SYSTEM_SCHEDULER_HPP
#define SYSTEM_SCHEDULER_HPP

#include <BonsaiBase.hpp>
#include <Containers/DynamicArray.hpp>
#include "System.hpp"

class World;

/// @brief System update priority.
enum SystemPriority : int32
{
    SystemPriority_Tick = 0,
};

/// @brief A SystemScheduler handles system registration and system updates according to priority.
class SystemScheduler
{
public:
    SystemScheduler(IAllocator* allocator = nullptr);

    /// @brief Register a system to the scheduler.
    /// @param priority 
    /// @param system 
    void Register(SystemPriority priority, std::unique_ptr<ISystem>&& system);

    /// @brief Run all registered systems in their defined system priority.
    void RunSystems(World& world, float deltaTime) const;

private:
    struct SystemInfo
    {
        SystemPriority priority;
        std::unique_ptr<ISystem> system;
    };

    DynamicArray<SystemInfo> _systems;
};

#endif //SYSTEM_SCHEDULER_HPP
