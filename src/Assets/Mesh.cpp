#include "Mesh.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include "Assets/AssetImporter.hpp"

namespace Bonsai
{
    template<>
    std::shared_ptr<Mesh> AssetImporter<Mesh>::Import(std::filesystem::path const& path)
    {
        // Read OBJ file
        tinyobj::ObjReaderConfig config{};
        config.triangulate = true;
        config.triangulation_method = "earcut";

        tinyobj::ObjReader reader{};
        if (!reader.ParseFromFile(path.string(), config)) {
            return {};
        }

        // Get OBJ data
        std::vector<tinyobj::shape_t> const& shapes = reader.GetShapes();
        tinyobj::attrib_t const& attributes = reader.GetAttrib();

        // Read shapes into bonsai mesh data
        std::vector<Vertex> vertices{};
        std::vector<uint32_t> indices{};
        std::vector<SubMesh> subMeshes{};
        for (auto const& shape : shapes)
        {
            // Start new sub mesh
            SubMesh subMesh{};
            subMesh.vertexOffset = static_cast<uint32_t>(vertices.size());
            subMesh.indexOffset = static_cast<uint32_t>(indices.size());
            subMesh.vertexCount = 0;
            subMesh.indexCount = 0;

            // Read shape face data
            uint32_t faceBaseIndex = 0;
            for (auto const& faceVertexCount : shape.mesh.num_face_vertices)
            {
                // Parse face vertices
                for (uint32_t faceVertexIndex = 0; faceVertexIndex < faceVertexCount; faceVertexIndex++)
                {
                    // Calculate data offsets based on index
                    tinyobj::index_t const& index = shape.mesh.indices[faceBaseIndex + faceVertexIndex];
                    uint32_t const vertIndex = index.vertex_index * 3;
                    uint32_t const normalIndex = index.normal_index * 3;
                    uint32_t const texCoordIndex = index.texcoord_index * 2;
                    
                    // Read in vertex data
                    glm::vec3 const position = { attributes.vertices[vertIndex], attributes.vertices[vertIndex + 1], attributes.vertices[vertIndex + 2] };
                    glm::vec3 const normal = { attributes.normals[normalIndex], attributes.normals[normalIndex + 1], attributes.normals[normalIndex + 2] };
                    glm::vec2 const texCoords = { attributes.texcoords[texCoordIndex], attributes.texcoords[texCoordIndex + 1] };

                    // Set vertex data
                    Vertex vertex{};
                    vertex.position = position;
                    vertex.normal = normal;
                    vertex.tangent = {}; // FIXME(nemjit001): Set this after mesh loading, during post processing steps
                    vertex.texCoords = texCoords;

                    // Store vertex data in mesh buffers
                    vertices.push_back(vertex);
                    indices.push_back(static_cast<uint32_t>(indices.size()));
                }

                // Increment base index for next face
                faceBaseIndex += faceVertexCount;
            }

            // Set vertex and index counts for sub mesh
            subMesh.vertexCount = static_cast<uint32_t>(vertices.size()) - subMesh.vertexOffset;
            subMesh.indexCount = static_cast<uint32_t>(indices.size()) - subMesh.indexOffset;

            // Store sub mesh
            subMeshes.push_back(subMesh);
        }

        return std::make_shared<Mesh>(vertices, indices, subMeshes);
    }

    Mesh::Mesh(std::vector<Vertex> const& vertices, std::vector<uint32_t> const& indices, std::vector<SubMesh> const& subMeshes)
        :
        _vertices(vertices),
        _indices(indices),
        _subMeshes(subMeshes)
    {
        //
    }
} // namespace Bonsai
