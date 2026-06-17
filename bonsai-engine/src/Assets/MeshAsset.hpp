#pragma once
#ifndef MESH_ASSET_HPP
#define MESH_ASSET_HPP

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

/// @brief A SubMesh is a part of a mesh with its own material and vertex/index arrays.
class SubMesh
{
public:
    DynamicArray<Vertex> vertices;
    std::optional<DynamicArray<uint32>> indices;
};

/// @brief A MeshAsset is a collection of submeshes that build a single mesh object.
class MeshAsset
{
public:
    DynamicArray<SubMesh> meshes;
};

#endif //MESH_ASSET_HPP
