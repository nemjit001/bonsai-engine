#pragma once
#ifndef BONSAI_ECS_SYSTEM_HPP
#define BONSAI_ECS_SYSTEM_HPP

#include <entt/entt.hpp>

namespace Bonsai
{
    /// @brief The ECS System interface provides an abstract interface for implementing component behaviour.
    class IECSSystem
    {
    public:
        virtual ~IECSSystem() = default;

        /// @brief Run the ECS system on an EnTT registry.
        /// @param registry Registry to run the system on.
        virtual void Run(entt::registry& registry) = 0;
    };
} // namespace Bonsai

#endif //BONSAI_ECS_SYSTEM_HPP
