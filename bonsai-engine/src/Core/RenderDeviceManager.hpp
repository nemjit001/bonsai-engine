#pragma once
#ifndef RENDER_DEVICE_MANAGER_HPP
#define RENDER_DEVICE_MANAGER_HPP

#include <BonsaiBase.hpp>
#include <Containers/StringView.hpp>
#include <SDL3/SDL.h>
#include "EngineSubsystem.hpp"

/// @brief Initialization info for the render device manager.
struct RenderDeviceManagerInitInfo
{
    StringView windowTitle  = "Bonsai Engine"sv;
    uint32 windowWidth      = 1280u;
    uint32 windowHeight     = 720u;
};

/// @brief The RenderDeviceManager handles high level render device tasks in the engine (e.g. window management, marking frame boundaries).
class RenderDeviceManager : public IEngineSubsystem
{
public:
    /// @brief Initialize the render manager.
    /// @return 
    [[nodiscard]] bool Init(RenderDeviceManagerInitInfo const& initInfo);

    /// @brief Shut down the render manager.
    void Shutdown();

    /// @brief Start a new rendering frame.
    void NewFrame();

    /// @brief End the rendering frame.
    void EndFrame();

    virtual void ProcessEvent(SDL_Event const& event) override;

private:
    SDL_Window* _window = nullptr;
};

#endif //RENDER_DEVICE_MANAGER_HPP
