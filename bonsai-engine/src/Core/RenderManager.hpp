#pragma once
#ifndef RENDER_MANAGER_HPP
#define RENDER_MANAGER_HPP

#include "EngineSubsystem.hpp"

/// @brief The RenderManager handles high level render tasks in the engine.
class RenderManager : public IEngineSubsystem
{
public:
    /// @brief Initialize the render manager.
    /// @return 
    [[nodiscard]] bool Init();

    /// @brief Shut down the render manager.
    void Shutdown();

    /// @brief Start a new rendering frame.
    void NewFrame();

    /// @brief End the rendering frame.
    void EndFrame();

    virtual void ProcessEvent(SDL_Event const& event) override;
};

#endif //RENDER_MANAGER_HPP
