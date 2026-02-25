#pragma once
#ifndef BONSAI_CORE_SUBSYSTEMS_ENGINE_EVENT_HPP
#define BONSAI_CORE_SUBSYSTEMS_ENGINE_EVENT_HPP

namespace bonsai::subsystems
{
    /// @brief Engine event types. All engine events should include an event type.
    enum EngineEventType
    {
        BONSAI_ENGINE_EVENT_TYPE_NONE = 0x00,
        BONSAI_ENGINE_EVENT_TYPE_QUIT = 0x10,
    };

    /// @brief Engine event structure containing event data.
    struct EngineEvent
    {
        EngineEventType event_type;
    };
} // namespace bonsai::subsystems

#endif //BONSAI_CORE_SUBSYSTEMS_ENGINE_EVENT_HPP
