#include "bonsai/subsystems/ecs_manager.hpp"

#include "bonsai/subsystems/scene_manager.hpp"

namespace bonsai::subsystems
{
    ECSManager::ECSManager(SceneManager* scene_manager)
        :
        m_scene_manager(scene_manager)
    {
        //
    }

    void ECSManager::run()
    {
        assets::Scene& active_scene = m_scene_manager->get_scene();
        for (auto const& system : m_ecs_systems)
        {
            if (system == nullptr) {
                continue;
            }

            system->run(active_scene.registry);
        }
    }
    
    ECSManager& ECSManager::register_system(IECSSystem* system)
    {
        if (std::find(std::begin(m_ecs_systems), std::end(m_ecs_systems), system) == std::end(m_ecs_systems)) {
            m_ecs_systems.push_back(system);
        }

        return *this;
    }

    void ECSManager::remove_system(IECSSystem* system)
    {
        m_ecs_systems.remove(system);
    }
} // namespace bonsai::subsystems
