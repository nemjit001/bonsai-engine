#pragma once
#ifndef BONSAI_RENDER_MESH_COMPONENT_HPP
#define BONSAI_RENDER_MESH_COMPONENT_HPP

#include "Gfx/GraphicsDevice.hpp"

namespace Bonsai
{
    /// @brief The Render Mesh Component stores graphics buffers for mesh data.
    struct RenderMeshComponent
    {
        GraphicsBuffer* vertexBuffer;
        GraphicsBuffer* indexBuffer;
    };
} // namespace Bonsai

#endif //BONSAI_RENDER_MESH_COMPONENT_HPP
