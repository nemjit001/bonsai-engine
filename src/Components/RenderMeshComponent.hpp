#pragma once
#ifndef BONSAI_RENDER_MESH_COMPONENT_HPP
#define BONSAI_RENDER_MESH_COMPONENT_HPP

#include "Gfx/GraphicsDevice.hpp"

namespace Bonsai
{
    /// @brief The Render Mesh Component stores graphics buffers for mesh data.
    // TODO(nemjit001): Replace buffers in component with handle into a graphics resource store to avoid duplicating mesh data in GPU memory
    struct RenderMeshComponent
    {
        GraphicsBuffer* vertexBuffer;
        GraphicsBuffer* indexBuffer;
    };
} // namespace Bonsai

#endif //BONSAI_RENDER_MESH_COMPONENT_HPP
