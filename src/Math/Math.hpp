#pragma once
#ifndef BONSAI_MATH_HPP
#define BONSAI_MATH_HPP

#define GLM_FORCE_RADIANS
#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Math
{
    static constexpr glm::vec3 WorldForward = glm::vec3(0.0F, 0.0F, 1.0F);
    static constexpr glm::vec3 WorldRight   = glm::vec3(1.0F, 0.0F, 0.0F);
    static constexpr glm::vec3 WorldUp      = glm::vec3(0.0F, 1.0F, 0.0F);
};

#endif //BONSAI_MATH_HPP
