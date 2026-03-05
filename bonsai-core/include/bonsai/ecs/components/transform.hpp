#pragma once
#ifndef BONSAI_CORE_ECS_COMPONENTS_TRANSFORM
#define BONSAI_CORE_ECS_COMPONENTS_TRANSFORM

#include <cereal/cereal.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

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
            _position[3] = { position.x, position.y, position.z };
            _rotation[4] = { rotation.w, rotation.x, rotation.y, rotation.z };
            _scale[3] = { scale.x, scale.y, scale.z };

            archive(
                cereal::make_nvp("position", _position),
                cereal::make_nvp("rotation", _rotation),
                cereal::make_nvp("scale", _scale)
            );

            position = glm::make_vec3(_position);
            rotation = glm::make_quat(_rotation);
            scale = glm::make_vec3(_scale);
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
