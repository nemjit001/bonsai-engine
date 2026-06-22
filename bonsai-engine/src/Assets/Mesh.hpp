#pragma once
#ifndef MESH_HPP
#define MESH_HPP

#include <optional>
#include <BonsaiBase.hpp>
#include <Containers/DynamicArray.hpp>
#include <Math.hpp>

/// @brief Static mesh vertex data.
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec2 texCoord;
};

/// @brief Vertex skinning weights.
struct VertexWeights
{
    glm::ivec4 boneIds;
    glm::vec4 boneWeights;
};

/// @brief A SubMesh is a part of a mesh with its own material and vertex/index arrays.
class SubMesh
{
public:
    AABB bounds;
    DynamicArray<Vertex> vertices;
    DynamicArray<VertexWeights> weights;
    DynamicArray<uint32> indices;
};

/// @brief A Mesh is a collection of submeshes that build a single mesh object.
class Mesh
{
public:
    AABB bounds;
    DynamicArray<SubMesh> meshes;
};

#endif //MESH_HPP
