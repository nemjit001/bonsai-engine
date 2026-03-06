#pragma once
#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "BonsaiMath.hpp"

/// @brief A Material provides surface parameters for meshes when they are rendered.
class Material
{
public:
    glm::vec3 Albedo    = { 0.5F, 0.5F, 0.5F };
    float Metallic      = 0.0F;
    float Roughness     = 0.5F;
};

#endif //MATERIAL_HPP
