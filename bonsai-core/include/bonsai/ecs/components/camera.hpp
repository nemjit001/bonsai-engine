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
        /// @param aspect_ratio Camera aspect ratio to use for the projection matrix.
        /// @return 
        inline glm::mat4 projection_matrix(float aspect_ratio) const;

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

    glm::mat4 Camera::projection_matrix(float aspect_ratio) const
    {
        return glm::perspective(glm::radians(fov), aspect_ratio, z_near, z_far);
    }
} // namespace bonsai::ecs

#endif  //BONSAI_CORE_ECS_COMPONENTS_CAMERA_HPP
