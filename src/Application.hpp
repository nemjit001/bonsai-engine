#pragma once
#ifndef BONSAI_APPLICATION_HPP
#define BONSAI_APPLICATION_HPP

#include <SDL3/SDL.h>
#include "Scene/SceneManager.hpp"

namespace Bonsai
{
    /// @brief Enumeration of return codes for the Application class.
    enum AppResult : int
    {
        AppResult_Ok        = 0,
        AppResult_BadInit   = 1,
    };

    /// @brief The Application class manages the application lifecyle.
    class Application
    {
    private:
        /// @brief Initialize the application.
        /// @return `true` on success.
        bool Init();

        /// @brief Shut down the application.
        void Shutdown();

        /// @brief Pump the platform message loop.
        void PumpMessages();
        
        /// @brief Handle a platform event in the application.
        /// @param event 
        void HandlePlatformEvent(SDL_Event const& event);

        /// @brief Tick the application state.
        void Tick();

        /// @brief Render an application frame.
        void Frame();

    public:
        /// @brief Run the application.
        /// @return 
        AppResult Run();

    private:
        bool _isRunning = false;
        SceneManager _sceneManager{};
    };
} // namespace Bonsai


#endif //BONSAI_APPLICATION_HPP
