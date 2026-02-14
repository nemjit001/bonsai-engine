#pragma once
#ifndef BONSAI_CORE_SUBSYSTEMS_SCENE_MANAGER_HPP
#define BONSAI_CORE_SUBSYSTEMS_SCENE_MANAGER_HPP

#include <optional>
#include "bonsai/core/filesystem.hpp"
#include "engine_subsystem.hpp"

namespace bonsai::subsystems
{
    /// @brief The scene subsystem manages scene loading and unloading, as well as updating the active scene state.
    class SceneManager : public IEngineSubsystem
    {
    public:
        /// @brief Initialize the scene manager.
        /// @param initial_scene Optional initial scene parameter. Not loading an initial scene will result in an empty scene.
        /// @return A boolean indicating successful initialization.
        [[nodiscard]] virtual bool init(std::optional<core::Path> const& initial_scene);
        void shutdown() override;
        void run() override;

    private:
        [[nodiscard]] bool init() override;
    };
} // namespace bonsai::subsystems

#endif //BONSAI_CORE_SUBSYSTEMS_SCENE_MANAGER_HPP
