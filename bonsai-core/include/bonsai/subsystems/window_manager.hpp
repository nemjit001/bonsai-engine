#pragma once
#ifndef BONSAI_CORE_SUBSYSTEMS_WINDOW_MANAGER_HPP
#define BONSAI_CORE_SUBSYSTEMS_WINDOW_MANAGER_HPP

#include <cstdint>
#include "engine_subsystem.hpp"

namespace bonsai::subsystems
{
    /// @brief The window subsystem manages platform window creation and window event processing.
    class WindowManager : public SingletonSubsystem<WindowManager>
    {
    private:
        friend SingletonSubsystem;
        WindowManager() = default;
    public:
        bool init() override;
        void shutdown() override;
        void run() override;
    };
} // namespace bonsai::subsystems

#endif //BONSAI_CORE_SUBSYSTEMS_WINDOW_MANAGER_HPP
