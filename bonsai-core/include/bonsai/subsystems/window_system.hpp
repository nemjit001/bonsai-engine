#pragma once
#ifndef BONSAI_CORE_SUBSYSTEMS_WINDOW_SYSTEM_HPP
#define BONSAI_CORE_SUBSYSTEMS_WINDOW_SYSTEM_HPP

#include <cstdint>
#include "engine_subsystem.hpp"

namespace bonsai::subsystems
{
    using WindowID = uint32_t;
    static constexpr WindowID INVALID_WINDOW_ID = ~0U;

    /// @brief The window subsystem manages platform window creation and window event processing.
    class WindowSystem : public IEngineSubsystem
    {
    public:
        bool init() override;

        void shutdown() override;

        void run() override;

        /// @brief Create a new window.
        /// @param title Window title.
        /// @param width Window width.
        /// @param height Window height.
        /// @return A window ID for the newly created window.
        WindowID create_window(char const* title, int32_t width, int32_t height);

        /// @brief Destroy a window.
        /// @param window_id Window ID for window to destroy.
        void destroy_window(WindowID window_id);
    };
} // namespace bonsai::subsystems

#endif //BONSAI_CORE_SUBSYSTEMS_WINDOW_SYSTEM_HPP
