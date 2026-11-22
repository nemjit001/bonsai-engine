#pragma once
#ifndef BONSAI_RENDER_WORLD_HPP
#define BONSAI_RENDER_WORLD_HPP

#include <unordered_map>
#include <vector>
#include <Gfx/GraphicsTypes.hpp>
#include "Assets/AssetManager.hpp"
#include "Assets/Mesh.hpp"
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
        std::unordered_map<AssetHandle<Mesh>, RenderMesh> geometryCache;
        std::vector<Shader::Camera> cameras;
        std::vector<Shader::RenderObject> renderObjects;
        std::vector<Shader::ObjectTransform> objectTransforms;
    };
} // namespace Bonsai

#endif //BONSAI_RENDER_WORLD_HPP
