#pragma once
#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "Base/PlatformTypes.hpp"

namespace bonsai
{
    /// @brief Return codes for engine initialization and run state.
    enum EngineResult : Int32
    {
        EngineResult_Ok         = 0,
        EngineResult_BadInit    = 1,
    };

    /// @brief The Engine class handles subsystem updates and the application loop.
    class Engine
    {
    public:
        /// @brief Run the engine main loop.
        /// @return 
        EngineResult Run();

        /// @brief Initialize the engine.
        /// @return 
        bool Init();

        /// @brief Shut down the engine.
        void Shutdown();

        /// @brief Tick the engine state.
        /// @param deltaTime 
        void Tick(double deltaTime);

    private:
        bool _isRunning = false;
    };
} // namespace bonsai

#endif //ENGINE_HPP
