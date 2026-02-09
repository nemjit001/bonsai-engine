#pragma once
#ifndef BONSAI_CORE_SUBSYSTEMS_ENGINE_SUBSYSTEM_HPP
#define BONSAI_CORE_SUBSYSTEMS_ENGINE_SUBSYSTEM_HPP

namespace bonsai::subsystems
{
    class IEngineSubsystem
    {
    public:
        virtual ~IEngineSubsystem() = default;

        virtual void init() = 0;

        virtual void shutdown() = 0;

        virtual void run() = 0;
    };
} // namespace bonsai::subsystems


#endif //BONSAI_CORE_SUBSYSTEMS_ENGINE_SUBSYSTEM_HPP
