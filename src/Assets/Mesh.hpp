#pragma once
#ifndef BONSAI_MESH_HPP
#define BONSAI_MESH_HPP

#include <cstdint>
#include <vector>
#include <Math/Math.hpp>
#include "Assets/AssetType.hpp"

namespace Bonsai
{
    class Material;

    /// @brief Vertex layout for bonsai meshes.
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 tangent;
        glm::vec2 texCoords;
    };

    /// @brief The SubMesh represents a range of mesh data that represents a loose mesh part.
    struct SubMesh
    {
        uint32_t materialId;
        uint32_t vertexOffset;
        uint32_t indexOffset;
        uint32_t vertexCount;
        uint32_t indexCount;
    };

    /// @brief The Mesh asset type contains host-side mesh data for rendering.
    class Mesh : public IAssetType
    {
    public:
        Mesh() = default;
        Mesh(
            std::vector<Vertex> const& vertices,
            std::vector<uint32_t> const& indices,
            std::vector<SubMesh> const& subMeshes,
            std::vector<Material> const& materials
        );

        /// @brief Get the dirty flag for the mesh. This will be set when host-side data is updated.
        /// @return The dirty flag for the mesh.
        bool GetDirtyFlag() const { return _isDirty; }

        /// @brief Set the dirty flag for the mesh.
        /// @param isDirty The new dirty flag value for the mesh.
        void SetDirtyFlag(bool isDirty) { _isDirty = isDirty; }

        /// @brief Get the mesh vertex buffer.
        /// @return The mesh vertices.
        std::vector<Vertex> const& GetVertices() const { return _vertices; }

        /// @brief Get the mesh index buffer.
        /// @return The mesh indices.
        std::vector<uint32_t> const& GetIndices() const { return _indices; }

        /// @brief Get the submeshes that make up the mesh.
        /// @return The list of submeshes.
        std::vector<SubMesh> const& GetSubmeshes() const { return _subMeshes; }

        /// @brief Get the materials that are attached to the mesh.
        /// @return The list of materials.
        std::vector<Material> const& GetMaterials() const { return _materials; }

    private:
        bool _isDirty = true;
        std::vector<Vertex> _vertices;
        std::vector<uint32_t> _indices;
        std::vector<SubMesh> _subMeshes;
        std::vector<Material> _materials;
    };
} // namespace Bonsai

#endif //BONSAI_MESH_HPP
