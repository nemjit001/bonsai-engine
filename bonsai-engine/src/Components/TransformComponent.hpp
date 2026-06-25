#pragma once
#ifndef TRANSFORM_COMPONENT_HPP
#define TRANSFORM_COMPONENT_HPP

#include "Math.hpp"

/// @brief A TransformComponent is used to place objects in the scene.
struct TransformComponent
{
    glm::vec3 position  = { 0.0F, 0.0F, 0.0F };
    glm::quat rotation  = { 1.0F, 0.0F, 0.0F, 0.0F };
    glm::vec3 scale     = { 1.0F, 1.0F, 1.0F };
};

#endif //TRANSFORM_COMPONENT_HPP
