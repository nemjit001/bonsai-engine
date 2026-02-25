#pragma once
#ifndef BONSAI_CORE_SUBSYSTEMS_ECS_MANAGER_HPP
#define BONSAI_CORE_SUBSYSTEMS_ECS_MANAGER_HPP

#include <list>
#include <entt/entt.hpp>
#include "engine_subsystem.hpp"

namespace bonsai::subsystems
{
    class SceneManager;

    /// @brief ECS system interface. ECS systems should implement this in order to be ran by the ECS manager.
    class IECSSystem
    {
    public:
        virtual ~IECSSystem() = default;

        /// @brief Run the ECS system.
        /// @param registry Registry of the active scene.
        virtual void run(entt::registry& registry) = 0;
    };

    /// @brief The ECS subsystem handles ECS system registration and execution.
    class ECSManager : public IEngineSubsystem
    {
    public:
        ECSManager(SceneManager* scene_manager);
        void run() override;

        /// @brief Register an ECS system with the ECS manager.
        /// @param system 
        /// @return 
        ECSManager& register_system(IECSSystem* system);
        
        /// @brief Remove an ECS system from the ECS manager.
        /// @param system 
        void remove_system(IECSSystem* system);

    private:
        SceneManager* m_scene_manager = nullptr;
        std::list<IECSSystem*> m_ecs_systems;
    };
} // namespace bonsai::subsystems

#endif //BONSAI_CORE_SUBSYSTEMS_ECS_MANAGER_HPP
