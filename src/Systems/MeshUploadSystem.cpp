#include "MeshUploadSystem.hpp"

#include <cassert>
#include "Core/Logger.hpp"
#include "Assets/Mesh.hpp"
#include "Components/MeshComponent.hpp"
#include "Components/RenderMeshComponent.hpp"

namespace Bonsai
{
    MeshUploadSystem::MeshUploadSystem(RenderManager* renderManager)
        :
        _renderManager(renderManager)
    {
        assert(_renderManager != nullptr && "renderManager was nullptr!");
    }

    void MeshUploadSystem::Run(entt::registry& registry)
    {
        // Get the graphics device
        GraphicsDevice* graphicsDevice = _renderManager->GetGraphicsDevice();
        assert(graphicsDevice != nullptr && "graphicsDevice was nullptr!");

        // Get the view into the registry
        auto view = registry.view<MeshComponent, RenderMeshComponent>();

        // Create mesh buffers for render meshes
        size_t requiredUploadBufferSize = 0;
        for (auto const& [ _, meshComponent, renderMeshComponent ] : view.each())
        {
            // Get stored host-side mesh
            std::shared_ptr<Mesh> mesh = meshComponent.MeshHandle.Get();
            if (mesh == nullptr) {
                continue;
            }

            // Check if mesh data is dirty
            if (!mesh->GetDirtyFlag()) {
                continue;
            }

            // Get vertex and index buffer sizes
            size_t const vertexBufferSize = sizeof(Vertex) * mesh->GetVertices().size();
            size_t const indexBufferSize = sizeof(uint32_t) * mesh->GetIndices().size();

            // Set up buffer descriptions
            GraphicsBufferDesc vertexBufferDesc{};
            vertexBufferDesc.heapType = GraphicsHeapType_Default;
            vertexBufferDesc.size = vertexBufferSize;
            vertexBufferDesc.usage = GraphicsBufferUsage_VertexBuffer | GraphicsBufferUsage_TransferDst;

            GraphicsBufferDesc indexBufferDesc{};
            indexBufferDesc.heapType = GraphicsHeapType_Default;
            indexBufferDesc.size = indexBufferSize;
            indexBufferDesc.usage = GraphicsBufferUsage_IndexBuffer | GraphicsBufferUsage_TransferDst;

            // Destroy old buffers
            graphicsDevice->DestroyBuffer(renderMeshComponent.vertexBuffer);
            graphicsDevice->DestroyBuffer(renderMeshComponent.indexBuffer);

            // Create new buffers
            renderMeshComponent.vertexBuffer = graphicsDevice->CreateBuffer(vertexBufferDesc);
            renderMeshComponent.indexBuffer = graphicsDevice->CreateBuffer(indexBufferDesc);

            // Update required upload buffer size
            requiredUploadBufferSize = vertexBufferSize + indexBufferSize;
        }

        // Check if uploads are required
        if (requiredUploadBufferSize == 0) {
            return;
        }

        // Create upload buffer for mesh data
        GraphicsBufferDesc uploadBufferDesc{};
        uploadBufferDesc.heapType = GraphicsHeapType_Upload;
        uploadBufferDesc.size = requiredUploadBufferSize;
        uploadBufferDesc.usage = GraphicsBufferUsage_TransferSrc;

        GraphicsBuffer* uploadBuffer = graphicsDevice->CreateBuffer(uploadBufferDesc);
        if (!uploadBuffer)
        {
            SPDLOG_WARN("Failed to create mesh data upload buffer!");
            return;
        }

        // Perform upload of host-side mesh data
        TransferBatch* meshDataTransferBatch = graphicsDevice->StartTransferBatch();
        GraphicsCommandBuffer* transferCommandBuffer = graphicsDevice->BeginTransferCommandBuffer(meshDataTransferBatch);

        size_t uploadBufferPointer = 0; // Used to track data location in upload buffer
        for (auto const& [ _, meshComponent, renderMeshComponent ] : view.each())
        {
            // Get stored host-side mesh
            std::shared_ptr<Mesh> mesh = meshComponent.MeshHandle.Get();
            if (mesh == nullptr) {
                continue;
            }

            // Check if mesh data is dirty
            if (!mesh->GetDirtyFlag()) {
                continue;
            }

            // Get vertex and index buffer sizes
            size_t const vertexBufferSize = sizeof(Vertex) * mesh->GetVertices().size();
            size_t const indexBufferSize = sizeof(uint32_t) * mesh->GetIndices().size();

            // Copy data to upload buffer
            size_t const vertexDataOffset = uploadBufferPointer;
            graphicsDevice->CopyToBuffer(uploadBuffer, mesh->GetVertices().data(), vertexBufferSize, uploadBufferPointer);
            uploadBufferPointer += vertexBufferSize;

            size_t const indexDataOffset = uploadBufferPointer;
            graphicsDevice->CopyToBuffer(uploadBuffer, mesh->GetIndices().data(), indexBufferSize, uploadBufferPointer);
            uploadBufferPointer += indexBufferSize;

            // Copy buffer data
            transferCommandBuffer->CopyBufferToBuffer(uploadBuffer, renderMeshComponent.vertexBuffer, vertexBufferSize, vertexDataOffset, 0);
            transferCommandBuffer->CopyBufferToBuffer(uploadBuffer, renderMeshComponent.indexBuffer, indexBufferSize, indexDataOffset, 0);

            // Clear dirty flag after recording upload command
            mesh->SetDirtyFlag(false);
        }

        graphicsDevice->SubmitTransferBatch(meshDataTransferBatch);
        graphicsDevice->WaitForTransferBatch(meshDataTransferBatch);
        graphicsDevice->DestroyBuffer(uploadBuffer);
    }
} // namespace Bonsai
