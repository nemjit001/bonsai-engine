#pragma once
#ifndef AABB_HPP
#define AABB_HPP

#include "MathTypes.hpp"

/// @brief An Axis Aligned Bounding Box represented by a min and max point.
class AABB
{
public:
    AABB() = default;
    AABB(glm::vec3 const& min, glm::vec3 const& max);

    /// @brief Get the AABB bounds min.
    /// @return 
    glm::vec3 Min() const { return _min; }

    /// @brief Get the AABB bounds max.
    /// @return 
    glm::vec3 Max() const { return _max; }

private:
    glm::vec3 _min = { 0.0F, 0.0F, 0.0F };
    glm::vec3 _max = { 0.0F, 0.0F, 0.0F };
};

#endif //AABB_HPP
