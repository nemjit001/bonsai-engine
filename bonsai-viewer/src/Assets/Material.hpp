#pragma once
#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "BonsaiMath.hpp"

/// @brief A Material provides surface parameters for meshes when they are rendered.
class Material
{
public:
    glm::vec3 Albedo;
    float Metallic;
    float Roughness;
};

#endif //MATERIAL_HPP
