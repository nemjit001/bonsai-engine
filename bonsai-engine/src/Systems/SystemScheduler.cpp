#include "SystemScheduler.hpp"

#include <algorithm>

void SystemScheduler::Register(SystemPriority priority, std::unique_ptr<ISystem>&& system)
{
    _systems.push_back(SystemInfo{ priority, std::move(system) });

    // By sorting on registration we avoid having to sort during system updates
    std::sort(_systems.begin(), _systems.end(), [](auto const& a, auto const& b) { return a.priority < b.priority; });
}

void SystemScheduler::RunSystems(World& world, float deltaTime) const
{
    for (auto const& info : _systems) {
        info.system->Run(world, deltaTime);
    }
}
