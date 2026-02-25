#pragma once
#ifndef BONSAI_CORE_SUBSYSTEMS_ECS_MANAGER_HPP
#define BONSAI_CORE_SUBSYSTEMS_ECS_MANAGER_HPP

#include "engine_subsystem.hpp"

namespace bonsai::subsystems
{
    /// @brief The ECS subsystem handles ECS system registration and execution.
    /// It executes ECS systems using a priority-queue.
    class ECSManager : public IEngineSubsystem
    {

    };
} // namespace bonsai::subsystems

#endif //BONSAI_CORE_SUBSYSTEMS_ECS_MANAGER_HPP
