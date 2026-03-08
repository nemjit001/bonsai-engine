#pragma once
#ifndef GPU_STATIC_MESH_HPP
#define GPU_STATIC_MESH_HPP

#include <vector>
#include "VulkanTypes.hpp"

/// @brief A GPUMeshOffset is an offset into static mesh buffers.
struct BGPUMeshOffset
{
    uint32_t VertexOffset;
    uint32_t IndexOffset;
    uint32_t VertexCount;
    uint32_t IndexCount;
};

/// @brief Vulkan GPU static mesh implementation.
class BGPUStaticMesh
{
public:
    std::vector<BGPUMeshOffset> SubMeshes;
    VulkanBuffer VertexBuffer;
    VulkanBuffer IndexBuffer;
};

#endif //GPU_STATIC_MESH_HPP
