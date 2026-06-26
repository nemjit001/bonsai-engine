#pragma once
#ifndef EVENT_LOOP_MANAGER_HPP
#define EVENT_LOOP_MANAGER_HPP

#include <BonsaiBase.hpp>
#include <Containers/DynamicArray.hpp>
#include "EngineSubsystem.hpp"

class EventLoopManager : public IEngineSubsystem
{
public:
    /// @brief Initialize the event loop manager.
    /// @param allocator Allocator to use for internal allocations.
    void Init(IAllocator* allocator = nullptr);

    /// @brief Shut down the event loop manager.
    void Shutdown();

    /// @brief Pump waiting platform events.
    void PumpMessages();

    /// @brief Register a subsystem for event processing.
    /// @param subsystem 
    void RegisterSubsystem(IEngineSubsystem* subsystem);

    /// @brief Check if the event loop is still running.
    /// @return 
    bool IsRunning() { return _isRunning; }

    void ProcessEvent(SDL_Event const& event) override;

private:
    bool _isRunning = false;
    DynamicArray<IEngineSubsystem*> _listeners;
};

#endif //EVENT_LOOP_MANAGER_HPP
