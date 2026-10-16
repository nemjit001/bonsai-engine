#pragma once
#ifndef BONSAI_MESH_HPP
#define BONSAI_MESH_HPP

#include <cstdint>
#include <vector>
#include "AssetType.hpp"
#include "Math/Math.hpp"

namespace Bonsai
{
    /// @brief Vertex layout for bonsai meshes.
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 tangent;
        glm::vec2 texCoords;
    };

    /// @brief The Mesh class contains a host-side mesh data for rendering.
    class Mesh : public IAssetType
    {
    public:
        Mesh() = default;
        Mesh(std::vector<Vertex> const& vertices, std::vector<uint32_t> const& indices);

        /// @brief Get the mesh vertex buffer.
        /// @return The mesh vertices.
        std::vector<Vertex> const& GetVertices() const { return _vertices; }

        /// @brief Get the mesh index buffer.
        /// @return The mesh indices.
        std::vector<uint32_t> const& GetIndices() const { return _indices; }

    private:
        std::vector<Vertex> _vertices;
        std::vector<uint32_t> _indices;
    };
} // namespace Bonsai

#endif //BONSAI_MESH_HPP
