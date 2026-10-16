#pragma once
#ifndef BONSAI_SYSTEM_REGISTRY_HPP
#define BONSAI_SYSTEM_REGISTRY_HPP

#include <memory>
#include <vector>
#include "Scene/ECSSystem.hpp"

namespace Bonsai
{
    class Scene;

    /// @brief The SystemRegistry manages a collection of ECS systems.
    class SystemRegistry
    {
    public:
        /// @brief Add an ECS System to the registry.
        /// @param args Arguments for the system constructor.
        template<typename SystemType, typename... Args>
        void AddSystem(Args&&... args);

        /// @brief Run the registered ECS systems on a scene.
        /// @param scene Scene to run systems on.
        void RunSystems(Scene& scene);
    private:
        std::vector<std::unique_ptr<IECSSystem>> _systems{};
    };

    template<typename SystemType, typename... Args>
    void SystemRegistry::AddSystem(Args&&... args)
    {
        static_assert(std::is_base_of_v<IECSSystem, SystemType>, "SystemType must inherit from IECSSystem!");
        _systems.emplace_back(std::make_unique<SystemType>(std::forward<Args>(args)...));
    }
} // namespace Bonsai

#endif //BONSAI_SYSTEM_REGISTRY_HPP
