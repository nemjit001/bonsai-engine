#pragma once
#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include "BonsaiMath.hpp"

/// @brief A Transform is used to give scene objects a position, rotation, and scale in the world.
class BTransform
{
public:
    /// @brief Get the transformation matrix for the Transform.
    /// @return 
    inline glm::mat4 GetMatrix() const {
        return glm::translate(glm::identity<glm::mat4>(), Position)
            * glm::mat4_cast(Rotation)
            * glm::scale(glm::identity<glm::mat4>(), Scale);
    }

    /// @brief Get the view matrix for the transform.
    /// @return 
    inline glm::mat4 GetViewMatrix() const {
        return glm::inverse(GetMatrix());
    }

public:
    glm::vec3 Position  = { 0.0F, 0.0F, 0.0F };
    glm::quat Rotation  = { 1.0F, 0.0F, 0.0F, 0.0F };
    glm::vec3 Scale     = { 1.0F, 1.0F, 1.0F };
};

#endif //TRANSFORM_HPP
