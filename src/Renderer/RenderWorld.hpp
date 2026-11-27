#pragma once
#ifndef BONSAI_RENDER_WORLD_HPP
#define BONSAI_RENDER_WORLD_HPP

#include <unordered_map>
#include <vector>
#include <Gfx/GraphicsTypes.hpp>
#include "Assets/AssetHandle.hpp"
#include "Assets/Mesh.hpp"
#include "Renderer/RenderObjectCache.hpp"
#include "Renderer/ShaderTypes.hpp"

namespace Bonsai
{
    /// @brief Render representation of a Bonsai mesh.
    struct RenderMesh
    {
        GraphicsBuffer* vertexBuffer;
        GraphicsBuffer* indexBuffer;
    };

    /// @brief Sort key for geometry draws in the render queue.
    struct GeometryDrawSortKey
    {
        uint64_t cameraId       : 8;    // 2^8 cameras
        uint64_t objectId       : 32;   // 2^32 render objects
        uint64_t meshId         : 24;   // 2^24 meshes per object
    };
    static_assert(sizeof(GeometryDrawSortKey) == sizeof(uint64_t), "Size of the GeometryDrawSortKey is not 64 bits!");
    
    /// @brief Geometry draw command in the render queue.
    struct GeometryDrawCommand
    {
        union SortKey
        {
            GeometryDrawSortKey fields;
            uint64_t value;
        } sortKey;
        RenderMesh renderMesh;
        SubMesh subMesh;
    };

    /// @brief The RenderWorld contains GPU render data.
    class RenderWorld
    {
    public:
        //-- render object and geometry tracking --//
        RenderObjectCache renderObjectCache;
        std::unordered_map<AssetHandle<Mesh>, RenderMesh> geometryCache;

        //-- host side data buffers --//
        std::vector<Shader::Camera> cameras;                    //< Camera shader data
        std::vector<Shader::RenderObject> renderObjects;        //< Render object shader data
        std::vector<Shader::ObjectTransform> objectTransforms;  //< Object transform shader data
        std::vector<Shader::SubMesh> subMeshes;                 //< SubMesh shader data
        std::vector<Shader::Material> materials;                //< Material shader data

        //-- device side data buffers --//
        SizedGraphicsBuffer cameraBuffer;
        SizedGraphicsBuffer renderObjectBuffer;
        SizedGraphicsBuffer objectTransformBuffer;
        SizedGraphicsBuffer subMeshBuffer;
        SizedGraphicsBuffer materialBuffer;

        //-- Render queues --//
        std::vector<GeometryDrawCommand> geometryRenderQueue;
    };
} // namespace Bonsai

#endif //BONSAI_RENDER_WORLD_HPP
