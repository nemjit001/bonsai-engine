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
} // namespace bonsai::ecs

#endif //BONSAI_CORE_ECS_COMPONENTS_TRANSFORM
