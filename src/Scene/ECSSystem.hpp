#pragma once
#ifndef BONSAI_ECS_SYSTEM_HPP
#define BONSAI_ECS_SYSTEM_HPP

namespace Bonsai
{
    class Scene;

    /// @brief The ECS System interface can be implemented to provide behaviour for ECS components.
    class IECSSystem
    {
    public:
        virtual ~IECSSystem() = default;

        /// @brief Run the ECS system on a scene.
        /// @param scene Scene to run the system on.
        virtual void Run(Scene& scene) = 0;
    };
} // namespace Bonsai

#endif //BONSAI_ECS_SYSTEM_HPP
