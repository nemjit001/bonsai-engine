#pragma once
#ifndef BONSAI_VIEWER_ASSETS_MESH_HPP
#define BONSAI_VIEWER_ASSETS_MESH_HPP

#include <vector>
#include <cereal/cereal.hpp>
#include <glm/glm.hpp>

namespace bonsai::assets
{
    /// @brief Static vertex layout for meshes.
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 tangent;
        glm::vec3 color;
        glm::vec2 tex_coord;
    };

    /// @brief The sub mesh class contains a single mesh segment.
    class SubMesh
    {
    public:
        template<class Archive>
        void serialize(Archive& archive);

    public:
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };

    /// @brief The mesh class is used to store mesh assets. Mesh assets are imported from DCC tools through mesh importers.
    class Mesh
    {
    public:
        template<class Archive>
        void serialize(Archive& archive);

    public:
        std::vector<SubMesh> sub_meshes;
    };

    template<class Archive>
    void SubMesh::serialize(Archive& archive)
    {
        archive(
            CEREAL_NVP(vertices),
            CEREAL_NVP(indices)
        );
    }

    template<class Archive>
    void Mesh::serialize(Archive& archive)
    {
        archive(CEREAL_NVP(sub_meshes));
    }
} // namespace bonsai::assets

#endif //BONSAI_VIEWER_MESH_HPP
