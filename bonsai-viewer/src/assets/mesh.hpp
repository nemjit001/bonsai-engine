#pragma once
#ifndef BONSAI_VIEWER_ASSETS_MESH_HPP
#define BONSAI_VIEWER_ASSETS_MESH_HPP

#include <vector>
#include <cereal/cereal.hpp>
#include <glm/glm.hpp>
#include "asset_loader.hpp"

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
        void serialize(Archive& archive)
        {
            archive(
                CEREAL_NVP(vertices),
                CEREAL_NVP(indices)
            );
        }

    public:
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };

    /// @brief The mesh class is used to store mesh assets. Mesh assets are imported from DCC tools through mesh importers.
    class Mesh
    {
    public:
        template<class Archive>
        void serialize(Archive& archive)
        {
            archive(CEREAL_NVP(sub_meshes));
        }

    public:
        std::vector<SubMesh> sub_meshes;
    };

    template<>
    std::optional<Mesh> AssetLoader<Mesh>::load(core::Path const& path);

    /// @brief Mesh importer interface for mesh importers to implement.
    class IMeshImporter
    {
    public:
        virtual ~IMeshImporter() = default;

        /// @brief Load a mesh stored in a DCC file format from disk.
        /// @param path 
        /// @return The loaded mesh or an empty object.
        virtual std::optional<Mesh> import(core::Path const& path) = 0;
    };

    /// @brief Mesh importer for Wavefront mesh files.
    class WavefrontMeshImporter : IMeshImporter
    {
    public:
        static constexpr char const* FILE_EXTENSION = ".obj";

        std::optional<Mesh> import(core::Path const& path) override;
    };
} // namespace bonsai::assets

#endif //BONSAI_VIEWER_MESH_HPP
