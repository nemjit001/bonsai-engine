#pragma once
#ifndef BONSAI_SCENE_MANAGER_HPP
#define BONSAI_SCENE_MANAGER_HPP

#include "Scene/Scene.hpp"

namespace Bonsai
{
    /// @brief The SceneManager manages the active scene and the ECS SystemRegistry.
    class SceneManager
    {
    public:
        /// @brief Initialize the scene manager.
        /// @return A boolean indicating success.
        bool Initialize();

        /// @brief Shut down the scene manager.
        void Shutdown();

        /// @brief Get the active scene from the scene manager.
        /// @return 
        Scene& GetActiveScene() { return _activeScene; }
        Scene const& GetActiveScene() const { return _activeScene; }

    private:
        Scene _activeScene{};
    };
} // namespace Bonsai

#endif //BONSAI_SCENE_MANAGER_HPP
