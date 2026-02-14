#pragma once
#ifndef BONSAI_CORE_SUBSYSTEMS_SCENE_MANAGER_HPP
#define BONSAI_CORE_SUBSYSTEMS_SCENE_MANAGER_HPP

#include <optional>
#include "bonsai/core/filesystem.hpp"
#include "engine_subsystem.hpp"

namespace bonsai::subsystems
{
    class SceneManager : public IEngineSubsystem
    {
    public:
        [[nodiscard]] virtual bool init(std::optional<core::Path> const& initial_scene);

    private:
        [[nodiscard]] bool init() override;
    };
} // namespace bonsai::subsystems

#endif //BONSAI_CORE_SUBSYSTEMS_SCENE_MANAGER_HPP
