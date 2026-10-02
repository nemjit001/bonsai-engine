#pragma once
#ifndef BONSAI_TRANSFORM_COMPONENT_HPP
#define BONSAI_TRANSFORM_COMPONENT_HPP

#include "Math/Math.hpp"

namespace Bonsai
{
    /// @brief The Transform Component contains a TRS transform for entities that places them in  the scene.
    /// An entity without a TransformComponent is assumed to have the identity transform.
    struct TransformComponent
    {
        glm::vec3 translation   = { 0.0F, 0.0F, 0.0F };
        glm::quat rotation      = { 1.0F, 0.0F, 0.0F, 0.0F };
        glm::vec3 scale         = { 1.0F, 1.0F, 1.0F };

        /// @brief Calculate the affine transformation matrix for this transform.
        /// @return The transformation matrix for this transform.
        glm::mat4 GetMatrix() const;
    };

    inline glm::mat4 TransformComponent::GetMatrix() const
    {
        glm::mat4 const translated = glm::translate(glm::identity<glm::mat4>(), translation);
        glm::mat4 const rotated = glm::mat4_cast(rotation) * translated;
        glm::mat4 const scaled = glm::scale(rotated, scale);
        return scaled;
    }
} // namespace Bonsai

#endif //BONSAI_TRANSFORM_COMPONENT_HPP
