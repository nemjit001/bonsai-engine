#pragma once
#ifndef BONSAI_CORE_SUBSYSTEMS_WINDOW_MANAGER_HPP
#define BONSAI_CORE_SUBSYSTEMS_WINDOW_MANAGER_HPP

#include <cstdint>
#include "engine_subsystem.hpp"

namespace bonsai::subsystems
{
    using WindowID = uint32_t;

    /// @brief Window client API to use, required for windowing backend configuration.
    enum class WindowClientAPI
    {
        OpenGL,
        Vulkan,
    };

    /// @brief The window config is used to specify extra attributes for created windows.
    struct WindowConfig
    {
        bool resizable;
        bool allow_high_dpi;
        WindowClientAPI client_api;
    };

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

        /// @brief Create a new window.
        /// @param title Window title.
        /// @param width Window width in pixels.
        /// @param height Window height in pixels.
        /// @param config Window config to use.
        /// @return A window ID referencing this window.
        WindowID create_window(char const* title, uint32_t width, uint32_t height, WindowConfig const& config);

        /// @brief Destroy a window.
        /// @param window 
        void destroy_window(WindowID window);

        /// @brief Get the window size in physical pixels.
        /// @param window 
        /// @param width 
        /// @param height 
        void get_window_size(WindowID window, uint32_t& width, uint32_t& height);
    };
} // namespace bonsai::subsystems

#endif //BONSAI_CORE_SUBSYSTEMS_WINDOW_MANAGER_HPP
