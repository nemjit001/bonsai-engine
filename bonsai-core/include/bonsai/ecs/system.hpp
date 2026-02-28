#pragma once
#ifndef BONSAI_CORE_ECS_SYSTEM_HPP
#define BONSAI_CORE_ECS_SYSTEM_HPP

#include <entt/entt.hpp>

namespace bonsai::ecs
{
    /// @brief ECS system interface. ECS systems should implement this in order to be ran by the ECS manager.
    class ISystem
    {
    public:
        virtual ~ISystem() = default;

        /// @brief Run the ECS system.
        /// @param registry Registry of the active scene.
        virtual void run(entt::registry& registry) = 0;
    };
} // namespace bonsai::ecs

#endif //BONSAI_CORE_ECS_SYSTEM_HPP
