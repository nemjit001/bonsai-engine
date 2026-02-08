#include "bonsai/assets/mesh.hpp"

#define TINY_OBJ_LOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace bonsai::assets
{
    template<>
    std::optional<Mesh> AssetLoader<Mesh>::load(core::Path const& path)
    {
        std::string const& ext = path.extension().string();
        if (ext == WavefrontMeshImporter::FILE_EXTENSION)
        {
            return WavefrontMeshImporter().import(path);
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

        if (reader.ParseFromFile(path.string(), config))
        {
            return {};
        }

        auto const& attrib = reader.GetAttrib();
        auto const& shapes = reader.GetShapes();
        // auto const& materials = reader.GetMaterials(); // TODO(nemjit001): also process materials in model file

        Mesh mesh{};
        for (auto const& shape : shapes)
        {
            uint32_t vertex_index = 0;
            SubMesh sub_mesh{};
            for (size_t face_idx = 0; face_idx < shape.mesh.num_face_vertices.size(); face_idx++)
            {
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
            
            mesh.sub_meshes.push_back(sub_mesh);
        }

        return mesh;
    }
} // namespace bonsai::assets

