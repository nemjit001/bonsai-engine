#pragma once
#ifndef BONSAI_CORE_SUBSYSTEMS_RENDER_MANAGER_HPP
#define BONSAI_CORE_SUBSYSTEMS_RENDER_MANAGER_HPP

#include "engine_subsystem.hpp"

namespace bonsai::subsystems
{
    class WindowManager;

    /// @brief The render manager handles internal rendering systems and renders a frame on update.
    class RenderManager : public IEngineSubsystem
    {
    public:
        RenderManager(WindowManager* window_manager);

    private:
        WindowManager* m_window_manager = nullptr;
    };
} // namespace bonsai::subsystems

#endif //BONSAI_CORE_SUBSYSTEMS_RENDER_MANAGER_HPP
