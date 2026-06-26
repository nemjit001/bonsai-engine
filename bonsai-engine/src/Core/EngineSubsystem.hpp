#pragma once
#ifndef ENGINE_SUBSYSTEM_HPP
#define ENGINE_SUBSYSTEM_HPP

#include <SDL3/SDL_events.h>

/// @brief The EngineSubsystem interface provides an abstract API that engine subsystems may implement (e.g. a RenderSubsystem, InputSubsystem).
class IEngineSubsystem
{
public:
    virtual ~IEngineSubsystem() = default;
};

#endif //ENGINE_SUBSYSTEM_HPP
