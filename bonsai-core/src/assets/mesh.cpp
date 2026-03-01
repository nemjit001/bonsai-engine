#include "bonsai/assets/mesh.hpp"

#define TINY_OBJ_LOADER_IMPLEMENTATION
#include <cstring>
#include <fstream>
#include <cereal/archives/portable_binary.hpp>
#include <tiny_obj_loader.h>

namespace bonsai::assets
{
    template<>
    std::optional<Mesh> AssetLoader<Mesh>::load(core::Path const& path)
    {
        std::string const ext = path.extension().string();
        if (ext == WavefrontMeshImporter::FILE_EXTENSION) {
            return WavefrontMeshImporter().import(path);
        }
        else if (ext == BonsaiMeshImporter::FILE_EXTENSION) {
            return BonsaiMeshImporter().import(path);
        }

        return {};
    }

    std::optional<Mesh> WavefrontMeshImporter::import(core::Path const& path)
    {
        tinyobj::ObjReader reader{};
        tinyobj::ObjReaderConfig config{};
        config.triangulate = true;
        config.triangulation_method = "earcut";
        config.vertex_color = true;

        if (reader.ParseFromFile(path.string(), config)) {
            return {};
        }

        tinyobj::attrib_t const& attrib = reader.GetAttrib();
        std::vector<tinyobj::shape_t> const& shapes = reader.GetShapes();
        std::vector<tinyobj::material_t> const& materials = reader.GetMaterials();

        Mesh mesh{};
        for (auto const& shape : shapes)
        {
            SubMesh sub_mesh{};
            Material material{};
            int current_material_id = -1;
            if (!shape.mesh.material_ids.empty())
            {
                current_material_id = shape.mesh.material_ids[0];
                material = process_material(materials[current_material_id]);
            }

            uint32_t vertex_index = 0;
            size_t const num_faces = shape.mesh.num_face_vertices.size();
            for (size_t face_idx = 0; face_idx < num_faces; face_idx++)
            {
                // Check if material changed for this face
                int const material_id = shape.mesh.material_ids[face_idx];
                if (material_id != current_material_id)
                {
                    // Store previous sub mesh with material
                    current_material_id = material_id; // Set material ID
                    vertex_index = 0; // Reset vertex index for sub mesh
                    mesh.sub_meshes.push_back(sub_mesh);
                    mesh.materials.push_back(material);

                    // Set up new material and sub mesh
                    sub_mesh = SubMesh{};
                    material = Material{};
                    if (current_material_id != -1) {
                        material = process_material(materials[current_material_id]);
                    }
                }

                // Read vertex data for face
                uint32_t const& num_face_verts = shape.mesh.num_face_vertices[face_idx];
                for (uint32_t i = 0; i < num_face_verts; i++)
                {
                    auto const& index = shape.mesh.indices[face_idx + i];
                    size_t const vert_base_idx = index.vertex_index * 3;
                    size_t const normal_base_idx = index.normal_index * 3;
                    size_t const texcoord_base_idx = index.texcoord_index * 2;

                    glm::vec3 const vertex_position = { attrib.vertices[vert_base_idx + 0], attrib.vertices[vert_base_idx + 1], attrib.vertices[vert_base_idx + 2], };
                    glm::vec3 const vertex_normal = { attrib.normals[normal_base_idx + 0], attrib.normals[normal_base_idx + 1], attrib.normals[normal_base_idx + 2], };
                    glm::vec3 const vertex_color = { attrib.colors[vert_base_idx + 0], attrib.colors[vert_base_idx + 1], attrib.colors[vert_base_idx + 2], };
                    glm::vec2 const vertex_texcoord = { attrib.texcoords[texcoord_base_idx + 0], attrib.texcoords[texcoord_base_idx + 1], };

                    sub_mesh.vertices.push_back(Vertex{ vertex_position, vertex_normal, {} /* empty tangent */, vertex_color, vertex_texcoord });
                    sub_mesh.indices.push_back(vertex_index + i);
                }

                vertex_index += num_face_verts;
            }

            // Push last active mesh and material
            mesh.sub_meshes.push_back(sub_mesh);
            mesh.materials.push_back(material);
        }

        return mesh;
    }

    Material WavefrontMeshImporter::process_material(tinyobj::material_t const& material) const
    {
        Material bonsai_mat{};
        std::memcpy(bonsai_mat.albedo, material.diffuse, sizeof(bonsai_mat.albedo));
        bonsai_mat.metallic = material.metallic;
        bonsai_mat.roughness = material.roughness;

        return bonsai_mat;
    }

    std::optional<Mesh> BonsaiMeshImporter::import(core::Path const& path)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            return {};
        }

        cereal::PortableBinaryInputArchive archive(file);
        Mesh mesh{};
        archive(CEREAL_NVP(mesh));
        return mesh;
    }
} // namespace bonsai::assets
