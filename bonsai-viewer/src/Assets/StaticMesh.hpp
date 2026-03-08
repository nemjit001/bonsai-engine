#pragma once
#ifndef STATIC_MESH_HPP
#define STATIC_MESH_HPP

#include <cstdint>
#include <vector>
#include "BonsaiMath.hpp"

/// @brief The default vertex layout for static meshes.
struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec3 Tangent;
    glm::vec3 Color;
    glm::vec2 TexCoord;
};

/// @brief A StaticMeshPart is used to represent a part of a static mesh that has a single material applied to it.
class BStaticMeshPart
{
public:
    std::vector<Vertex> Vertices;
    std::vector<uint32_t> Indices;
};

/// @brief A StaticMesh is a collection of StaticMeshParts that together form a single object.
class BStaticMesh
{
public:
    std::vector<BStaticMeshPart> SubMeshes;
};

#endif //STATIC_MESH_HPP
