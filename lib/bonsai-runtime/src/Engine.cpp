#include "Engine.hpp"

namespace bonsai
{
    EngineResult Engine::Run()
    {
        EngineResult const initResult = Init();
        if (initResult != EngineResult_Ok) {
            return initResult;
        }

        // Enter main loop
        while (_isRunning)
        {
            Tick();
        }

        Shutdown();
        return EngineResult_Ok;
    }

    EngineResult Engine::Init()
    {
        _isRunning = true;
        return EngineResult_Ok;
    }

    void Engine::Shutdown()
    {
        _isRunning = false;
    }

    void Engine::Tick()
    {
        //
    }
} // namespace bonsai
