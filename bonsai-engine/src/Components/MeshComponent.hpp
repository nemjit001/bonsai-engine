#pragma once
#ifndef MESH_COMPONENT_HPP
#define MESH_COMPONENT_HPP

#include <BonsaiBase.hpp>
#include "Assets/Mesh.hpp"

/// @brief A MeshComponent is used to provide a mesh for entities.
struct MeshComponent
{
    std::shared_ptr<Mesh> mesh;
};

#endif //MESH_COMPONENT_HPP
