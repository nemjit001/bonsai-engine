#pragma once
#ifndef BONSAI_VIEWER_ASSETS_MESH_HPP
#define BONSAI_VIEWER_ASSETS_MESH_HPP

#include <vector>
#include <cereal/cereal.hpp>
#include <glm/glm.hpp>

namespace bonsai::assets
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 tangent;
        glm::vec2 tex_coord;
    };

    class Mesh
    {
    public:
        template<class Archive>
        void serialize(Archive& archive);

    public:
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };

    template<class Archive>
    void Mesh::serialize(Archive& archive)
    {
        archive(
            CEREAL_NVP(vertices),
            CEREAL_NVP(indices)
        );
    }
} // namespace bonsai::assets

#endif //BONSAI_VIEWER_MESH_HPP
