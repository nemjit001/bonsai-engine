#pragma once
#ifndef BONSAI_SYSTEM_REGISTRY_HPP
#define BONSAI_SYSTEM_REGISTRY_HPP

#include <memory>
#include <vector>
#include "Systems/ECSSystem.hpp"

namespace Bonsai
{
    class Scene;

    /// @brief The SystemRegistry manages a collection of ECS systems and implements
    /// system scheduling.
    class SystemRegistry
    {
    public:
        /// @brief Add an ECS System to the registry.
        /// @param args Arguments for the system constructor.
        template<typename SystemType, typename... Args>
        void AddSystem(Args&&... args);

        /// @brief Run the registered ECS systems on a registry.
        /// @param registry Registry to run the systems on.
        void RunSystems(entt::registry& registry) const;

        /// @brief Clear all systems from the registry.
        void Clear();

    private:
        std::vector<std::unique_ptr<ECSSystem>> _systems{};
    };

    template<typename SystemType, typename... Args>
    void SystemRegistry::AddSystem(Args&&... args)
    {
        static_assert(std::is_base_of_v<ECSSystem, SystemType>, "SystemType must inherit from ECSSystem!");
        _systems.emplace_back(std::make_unique<SystemType>(std::forward<Args>(args)...));
    }
} // namespace Bonsai

#endif //BONSAI_SYSTEM_REGISTRY_HPP
