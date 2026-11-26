#pragma once
#ifndef BONSAI_MESH_COMPONENT_HPP
#define BONSAI_MESH_COMPONENT_HPP

#include "Assets/AssetHandle.hpp"
#include "Assets/Mesh.hpp"

namespace Bonsai
{
    /// @brief The Mesh Component stores a host-side mesh for an entity.
    struct MeshComponent
    {
        AssetHandle<Mesh> meshHandle = AssetHandle<Mesh>();
    };
} // namespace Bonsai

#endif //BONSAI_MESH_COMPONENT_HPP
