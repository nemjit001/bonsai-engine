#pragma once
#ifndef BONSAI_CORE_ENGINE_RESULT_HPP
#define BONSAI_CORE_ENGINE_RESULT_HPP

#include <cstdint>

namespace bonsai
{
    /// @brief The engine result enum is used to indicate engine state on program exit.
    enum EngineResult : int32_t
    {
        BONSAI_ENGINE_RESULT_OK                             = 0x0,
        BONSAI_ENGINE_RESULT_LOGGING_MANAGER_BAD_INIT       = 0x10,
        BONSAI_ENGINE_RESULT_ENGINE_EVENT_MANAGER_BAD_INIT  = 0x20,
        BONSAI_ENGINE_RESULT_WINDOW_MANAGER_BAD_INIT        = 0x30,
        BONSAI_ENGINE_RESULT_SCENE_MANAGER_BAD_INIT         = 0x40,
        BONSAI_ENGINE_RESULT_RENDER_MANAGER_BAD_INIT        = 0x50,
        BONSAI_ENGINE_RESULT_MAIN_LOOP_MANAGER_BAD_INIT     = 0x60,
    };
} // namespace bonsai

#endif //BONSAI_CORE_ENGINE_RESULT_HPP
