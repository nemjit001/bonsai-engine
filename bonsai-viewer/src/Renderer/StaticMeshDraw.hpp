#pragma once
#ifndef STATIC_MESH_DRAW_HPP
#define STATIC_MESH_DRAW_HPP

#include "RenderHandle.hpp"

class BGPUStaticMesh;
class BPipelineState;

/// @brief A StaticMeshDraw informs the renderer that a static mesh participates in the active frame.
struct StaticMeshDraw
{
    TRenderHandle<BPipelineState> PSO;
    TRenderHandle<BGPUStaticMesh> Mesh;
};

#endif //STATIC_MESH_DRAW_HPP
