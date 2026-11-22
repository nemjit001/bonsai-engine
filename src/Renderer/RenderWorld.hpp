#pragma once
#ifndef BONSAI_RENDER_WORLD_HPP
#define BONSAI_RENDER_WORLD_HPP

#include <unordered_map>
#include <vector>
#include <entt/entt.hpp>
#include <Gfx/GraphicsTypes.hpp>
#include "Assets/AssetManager.hpp"
#include "Assets/Mesh.hpp"
#include "Renderer/ShaderTypes.hpp"

namespace Bonsai
{
    /// @brief The RenderObjectId is an internal ID for referencing render objects.
    using RenderObjectId = uint32_t;

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
        RenderObjectId nextObjectId = 0;
        std::vector<RenderObjectId> renderObjectFreeList;
        std::unordered_map<entt::entity, RenderObjectId> renderObjectCache; //< Cache of entity ids mapped to stable render object ids
        std::unordered_map<AssetHandle<Mesh>, RenderMesh> geometryCache;    //< Cache of scene geometry currently in use
    };
} // namespace Bonsai

#endif //BONSAI_RENDER_WORLD_HPP
