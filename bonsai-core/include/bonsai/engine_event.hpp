#pragma once
#ifndef BONSAI_CORE_ENGINE_EVENT_HPP
#define BONSAI_CORE_ENGINE_EVENT_HPP

namespace bonsai
{
    /// @brief Engine event types. All engine events should include an event type.
    enum EngineEventType
    {
        BONSAI_ENGINE_EVENT_TYPE_QUIT = 0,
    };

    /// @brief Engine event structure containing event data.
    struct EngineEvent
    {
        EngineEventType event_type;
    };
} // namespace bonsai

#endif //BONSAI_CORE_ENGINE_EVENT_HPP
