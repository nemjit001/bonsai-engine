#pragma once
#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "Base/PlatformTypes.hpp"

namespace bonsai
{
    enum EngineResult : Int32
    {
        EngineResult_Ok = 0,
    };

    class Engine
    {
    public:
        EngineResult Run();

    private:
        EngineResult Init();

        void Shutdown();

        void Tick();

        bool _isRunning = false;
    };
} // namespace bonsai

#endif //ENGINE_HPP
