#include "Engine.hpp"

namespace bonsai
{
    EngineResult Engine::Run()
    { 
        if (!Init()) {
            return EngineResult_BadInit;
        }

        // Enter main loop
        while (_isRunning)
        {
            Tick(0.0);
        }

        Shutdown();
        return EngineResult_Ok;
    }

    bool Engine::Init()
    {
        _isRunning = true;
        return true;
    }

    void Engine::Shutdown()
    {
        _isRunning = false;
    }

    void Engine::Tick([[maybe_unused]] double deltaTime)
    {
        //
    }
} // namespace bonsai
