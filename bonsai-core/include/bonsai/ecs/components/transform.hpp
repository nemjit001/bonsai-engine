#pragma once
#ifndef BONSAI_CORE_ECS_COMPONENTS_TRANSFORM
#define BONSAI_CORE_ECS_COMPONENTS_TRANSFORM

#include <cereal/cereal.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace bonsai::ecs
{
    /// @brief Scene transform component, used to position entities in the scene.
    struct Transform
    {
        glm::vec3 position  = { 0.0F, 0.0F, 0.0F };
        glm::quat rotation  = { 1.0F, 0.0F, 0.0F, 0.0F };
        glm::vec3 scale     = { 1.0F, 1.0F, 1.0F };

        /// @brief Get the transformation matrix for this transform.
        /// @return 
        inline glm::mat4 matrix() const;

        /// @brief Get the view matrix for this transform. The view matrix is the inverse of the world transform, i.e. it sees the world from the view of this transform.
        /// @return 
        inline glm::mat4 view_matrix() const;

        template<class Archive>
        void serialize(Archive& archive)
        {
            archive(
                cereal::make_nvp("position", cereal::binary_data(&position[0], sizeof(position))),
                cereal::make_nvp("rotation", cereal::binary_data(&rotation[0], sizeof(rotation))),
                cereal::make_nvp("scale", cereal::binary_data(&scale[0], sizeof(scale))),
            )
        }
    };

    glm::mat4 Transform::matrix() const
    {
        return glm::translate(glm::identity<glm::mat4>(), position)
            * glm::mat4_cast(rotation)
            * glm::scale(glm::identity<glm::mat4>(), scale);
    }

    glm::mat4 Transform::view_matrix() const
    {
        glm::mat4 const transform = matrix();
        return glm::inverse(transform);
    }
} // namespace bonsai::ecs

#endif //BONSAI_CORE_ECS_COMPONENTS_TRANSFORM
