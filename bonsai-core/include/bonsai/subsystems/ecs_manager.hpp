#pragma once
#ifndef BONSAI_CORE_SUBSYSTEMS_ECS_MANAGER_HPP
#define BONSAI_CORE_SUBSYSTEMS_ECS_MANAGER_HPP

#include <list>
#include "bonsai/ecs/system.hpp"
#include "engine_subsystem.hpp"

namespace bonsai::subsystems
{
    class SceneManager;

    /// @brief The ECS subsystem handles ECS system registration and execution.
    class ECSManager : public IEngineSubsystem
    {
    public:
        ECSManager(SceneManager* scene_manager);
        void run() override;

        /// @brief Register an ECS system with the ECS manager.
        /// @param system 
        /// @return 
        ECSManager& register_system(ecs::ISystem* system);
        
        /// @brief Remove an ECS system from the ECS manager.
        /// @param system 
        void remove_system(ecs::ISystem* system);

    private:
        SceneManager* m_scene_manager = nullptr;
        std::list<ecs::ISystem*> m_ecs_systems;
    };
} // namespace bonsai::subsystems

#endif //BONSAI_CORE_SUBSYSTEMS_ECS_MANAGER_HPP
