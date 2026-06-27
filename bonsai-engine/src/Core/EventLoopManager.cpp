#include "EventLoopManager.hpp"

bool EventLoopManager::Init(IAllocator* allocator)
{
    _isRunning = true;
    _listeners = DynamicArray<IEngineSubsystem*>(ResolveAllocator(allocator));
    return true;
}

void EventLoopManager::Shutdown()
{
    _listeners.clear();
    _isRunning = false;
}

void EventLoopManager::PumpMessages()
{
    SDL_Event event{};
    while (_isRunning && SDL_PollEvent(&event))
    {
        for (auto const& listener : _listeners)
        {
            if (listener) {
                listener->ProcessEvent(event);
            }
        }
    }
}

void EventLoopManager::RegisterSubsystem(IEngineSubsystem* subsystem)
{
    _listeners.push_back(subsystem);
}

void EventLoopManager::ProcessEvent(SDL_Event const& event)
{
    if (event.type == SDL_EVENT_QUIT) {
        _isRunning = false;
    }
}
