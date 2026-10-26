#pragma once
#ifndef BONSAI_MESH_COMPONENT_HPP
#define BONSAI_MESH_COMPONENT_HPP

#include "Assets/AssetManager.hpp"

namespace Bonsai
{
    class Mesh;

    /// @brief The Mesh Component stores a mesh for an entity.
    struct MeshComponent
    {
        AssetHandle<Mesh> MeshHandle = AssetHandle<Mesh>();
    };
} // namespace Bonsai

#endif //BONSAI_MESH_COMPONENT_HPP
