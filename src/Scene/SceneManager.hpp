#pragma once
#ifndef BONSAI_SCENE_MANAGER_HPP
#define BONSAI_SCENE_MANAGER_HPP

#include "Scene.hpp"
#include "SystemRegistry.hpp"

namespace Bonsai
{
    /// @brief The SceneManager manages the active scene and the ECS SystemRegistry.
    class SceneManager
    {
    public:
        /// @brief Tick the SceneManager, running the registered ECS systems on the scene.
        void Tick();

        /// @brief Get the ECS System Registry.
        /// @return 
        SystemRegistry& GetSystemRegistry() { return _systemRegistry; }

    private:
        Scene _activeScene{};
        SystemRegistry _systemRegistry{};
    };
} // namespace Bonsai

#endif //BONSAI_SCENE_MANAGER_HPP
