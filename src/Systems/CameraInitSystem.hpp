#pragma once
#ifndef BONSAI_CAMERA_INIT_SYSTEM_HPP
#define BONSAI_CAMERA_INIT_SYSTEM_HPP

#include "Systems/ECSSystem.hpp"

namespace Bonsai
{
    /// @brief The CameraInitSystem is used to initialize at least 1 camera in the registry if it does not already exist.
    class CameraInitSystem : public ECSSystem
    {
    public:
        void Run(entt::registry& registry) override;
    };
} // namespace Bonsai

#endif //BONSAI_CAMERA_INIT_SYSTEM_HPP
