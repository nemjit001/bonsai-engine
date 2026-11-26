#include "Mesh.hpp"

#define TINYOBJLOADER_IMPLEMENTATION

#include <tiny_obj_loader.h>
#include <Core/Logger.hpp>
#include "Assets/AssetImporter.hpp"
#include "Assets/AssetManager.hpp"
#include "Assets/Material.hpp"

namespace Bonsai
{
    template<>
    std::shared_ptr<Mesh> AssetImporter<Mesh>::Import(AssetManager& assetManager, std::filesystem::path const& path)
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
        std::vector<tinyobj::material_t> const& materials = reader.GetMaterials(); 
        tinyobj::attrib_t const& attributes = reader.GetAttrib();

        // Read wavefront materials into Bonsai material data
        std::vector<AssetHandle<Material>> bonsaiMaterials{};
        for (auto const& material : materials)
        {
            SPDLOG_TRACE("Reading wavefront material: {}", material.name);

            // Read material parameters
            glm::vec3 const albedo = { material.diffuse[0], material.diffuse[1], material.diffuse[2], };
            float const metallic = material.metallic;
            float const roughness = material.roughness;

            // Create new material in asset manager and store in material vector
            AssetHandle<Material> bonsaiMaterial = assetManager.Create<Material>(albedo, metallic, roughness);
            bonsaiMaterials.push_back(bonsaiMaterial);
        }

        // Read shapes into Bonsai mesh data
        std::vector<Vertex> vertices{};
        std::vector<uint32_t> indices{};
        std::vector<SubMesh> subMeshes{};
        for (auto const& shape : shapes)
        {
            SPDLOG_TRACE("Reading wavefront shape: {}", shape.name);

            // Skip loading empty mesh shapes
            if (shape.mesh.num_face_vertices.empty())
            {
                SPDLOG_WARN("Shape {} has 0 faces!", shape.name);
                continue;
            }

            // Get active material ID for tracking sub mesh materials
            int32_t activeMaterialId = shape.mesh.material_ids[0];

            // Start new sub mesh
            SubMesh subMesh{};
            subMesh.materialId = static_cast<uint32_t>(activeMaterialId);
            subMesh.vertexOffset = static_cast<uint32_t>(vertices.size());
            subMesh.indexOffset = static_cast<uint32_t>(indices.size());
            subMesh.vertexCount = 0;
            subMesh.indexCount = 0;

            // Read shape face data
            size_t faceIndex = 0;
            uint32_t faceBaseIndex = 0;
            for (auto const& faceVertexCount : shape.mesh.num_face_vertices)
            {
                // Get material
                int32_t const materialId = shape.mesh.material_ids[faceIndex];

                // If material changed, save current sub-mesh and begin next sub-mesh
                if (materialId != activeMaterialId)
                {
                    // Store the next active material ID
                    activeMaterialId = materialId;

                    // Save previous sub-mesh state
                    subMesh.vertexCount = static_cast<uint32_t>(vertices.size()) - subMesh.vertexOffset;
                    subMesh.indexCount = static_cast<uint32_t>(indices.size()) - subMesh.indexOffset;
                    subMeshes.push_back(subMesh);

                    // Start new sub-mesh state
                    subMesh.materialId = static_cast<uint32_t>(activeMaterialId);
                    subMesh.vertexOffset = static_cast<uint32_t>(vertices.size());
                    subMesh.indexOffset = static_cast<uint32_t>(indices.size());
                    subMesh.vertexCount = 0;
                    subMesh.indexCount = 0;
                }

                // Parse face vertices
                for (uint32_t faceVertexIndex = 0; faceVertexIndex < faceVertexCount; faceVertexIndex++)
                {
                    // Calculate data offsets based on index
                    tinyobj::index_t const& index = shape.mesh.indices[faceBaseIndex + faceVertexIndex];
                    int32_t const vertIndex = index.vertex_index * 3;
                    int32_t const normalIndex = index.normal_index * 3;
                    int32_t const texCoordIndex = index.texcoord_index * 2;
                    
                    // Read in vertex data
                    Vertex vertex{};
                    vertex.position = { attributes.vertices[vertIndex], attributes.vertices[vertIndex + 1], attributes.vertices[vertIndex + 2] };
                    vertex.normal = {}; // Optionally set during import
                    vertex.tangent = {}; // Tangents will be calculated after import
                    vertex.texCoords = {}; // Optionally set during import

                    // Optionally read in vertex normals
                    if (normalIndex >= 0) {
                       vertex.normal = { attributes.normals[normalIndex], attributes.normals[normalIndex + 1], attributes.normals[normalIndex + 2] };
                    }

                    // Optionally read in vertex texture coords
                    if (texCoordIndex >= 0) {
                        vertex.texCoords = { attributes.texcoords[texCoordIndex], attributes.texcoords[texCoordIndex + 1] };
                    }

                    // Store vertex data in mesh buffers
                    vertices.push_back(vertex);
                    indices.push_back(static_cast<uint32_t>(indices.size()));
                }

                // Increment base index for next face
                faceBaseIndex += faceVertexCount;

                // Increment face index
                faceIndex++;
            }

            // Save sub-mesh state
            subMesh.vertexCount = static_cast<uint32_t>(vertices.size()) - subMesh.vertexOffset;
            subMesh.indexCount = static_cast<uint32_t>(indices.size()) - subMesh.indexOffset;
            subMeshes.push_back(subMesh);
        }

        return std::make_shared<Mesh>(vertices, indices, subMeshes, bonsaiMaterials);
    }

    Mesh::Mesh(
        std::vector<Vertex> const& vertices,
        std::vector<uint32_t> const& indices,
        std::vector<SubMesh> const& subMeshes,
        std::vector<AssetHandle<Material>> const& materials
    )
        :
        _vertices(vertices),
        _indices(indices),
        _subMeshes(subMeshes),
        _materials(materials)
    {
        //
    }
} // namespace Bonsai
