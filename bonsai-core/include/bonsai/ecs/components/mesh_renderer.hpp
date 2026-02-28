#pragma once
#ifndef BONSAI_CORE_ECS_COMPONENTS_MESH_RENDERER_HPP
#define BONSAI_CORE_ECS_COMPONENTS_MESH_RENDERER_HPP

#include <memory>
#include <cereal/cereal.hpp>
#include "bonsai/assets/mesh.hpp"

namespace bonsai::ecs
{
    /// @brief Mesh renderer component, used by entities that wish to render a mesh.
    struct MeshRenderer
    {
        std::shared_ptr<assets::Mesh> mesh;

        template<class Archive>
        void serialize(Archive& archive)
        {
            archive(CEREAL_NVP(mesh))
        }
    };
} // namespace bonsai::ecs

#endif //BONSAI_CORE_ECS_COMPONENTS_MESH_RENDERER_HPP
