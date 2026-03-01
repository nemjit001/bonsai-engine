#pragma once
#ifndef BONSAI_CORE_ECS_COMPONENTS_CAMERA_HPP
#define BONSAI_CORE_ECS_COMPONENTS_CAMERA_HPP

#include <cereal/cereal.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace bonsai::ecs
{
    struct Camera
    {
        float fov       = 60.0F; // Horizontal FOV in degrees
        float z_near    = 0.01F;
        float z_far     = 100.0F;

        /// @brief Get the projection matrix for this camera.
        /// @return 
        inline glm::mat4 projection_matrix() const;

        template<class Archive>
        void serialize(Archive& archive)
        {
            archive(
                CEREAL_NVP(fov),
                CEREAL_NVP(z_near),
                CEREAL_NVP(z_far),
            );
        }
    };

    glm::mat4 Camera::projection_matrix() const
    {
        // TODO(nemjit001): Use a render target handle for fetching aspect ratio?
        return glm::perspective(glm::radians(fov), 1.0F, z_near, z_far);
    }
} // namespace bonsai::ecs

#endif  //BONSAI_CORE_ECS_COMPONENTS_CAMERA_HPP
