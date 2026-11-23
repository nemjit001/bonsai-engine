#pragma once
#ifndef BONSAI_RENDER_WORLD_HPP
#define BONSAI_RENDER_WORLD_HPP

#include <unordered_map>
#include <vector>
#include <Gfx/GraphicsTypes.hpp>
#include "Assets/AssetManager.hpp"
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

        //-- device side data buffers --//
        size_t cameraBufferSize = 0;
        size_t renderObjectBufferSize = 0;
        size_t objectTransformBufferSize = 0;
        GraphicsBuffer* cameraBuffer = nullptr;
        GraphicsBuffer* renderObjectBuffer = nullptr;
        GraphicsBuffer* objectTransformBuffer = nullptr;
    };
} // namespace Bonsai

#endif //BONSAI_RENDER_WORLD_HPP
