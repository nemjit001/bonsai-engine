#pragma once
#ifndef BONSAI_CORE_ASSETS_MESH_HPP
#define BONSAI_CORE_ASSETS_MESH_HPP

#include <vector>
#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp> // Required for std::vector serialization
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <tiny_obj_loader.h>
#include "asset_loader.hpp"
#include "material.hpp"

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

        template<class Archive>
        void serialize(Archive& archive)
        {
            // NOTE(nemjit001): Binary serialization allows skipping the serialize implementation for math types
            archive(
                cereal::make_nvp("position", cereal::binary_data(glm::value_ptr(position), sizeof(position))),
                cereal::make_nvp("normal", cereal::binary_data(glm::value_ptr(normal), sizeof(normal))),
                cereal::make_nvp("tangent", cereal::binary_data(glm::value_ptr(tangent), sizeof(tangent))),
                cereal::make_nvp("color", cereal::binary_data(glm::value_ptr(color), sizeof(color))),
                cereal::make_nvp("tex_coord", cereal::binary_data(glm::value_ptr(tex_coord), sizeof(tex_coord)))
            );
        }
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
    /// Meshes store their materials internally as loaded from DCC tools.
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
        std::vector<Material> materials;
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

    private:
        Material process_material(tinyobj::material_t const& material) const;
    };

    /// @brief Mesh importer for Bonsai serialized mesh files.
    class BonsaiMeshImporter : IMeshImporter
    {
    public:
        static constexpr char const* FILE_EXTENSION = ".mesh";

        std::optional<Mesh> import(core::Path const& path) override;
    };
} // namespace bonsai::assets

#endif //BONSAI_CORE_ASSETS_MESH_HPP
