#include "UploadRenderWorldSystem.hpp"

#include <cassert>
#include <Core/Logger.hpp>
#include <Gfx/GraphicsDevice.hpp>
#include "Renderer/RenderManager.hpp"

namespace Bonsai
{
    UploadRenderWorldSystem::UploadRenderWorldSystem(RenderManager* renderManager)
        :
        _renderManager(renderManager)
    {
        assert(_renderManager != nullptr && "renderManager was null!");
    }

    UploadRenderWorldSystem::~UploadRenderWorldSystem()
    {
        // Destroy upload buffer
        GraphicsDevice* graphicsDevice = _renderManager->GetGraphicsDevice();
        graphicsDevice->DestroyBuffer(_sceneDataUploadBuffer);
    }

    void UploadRenderWorldSystem::Run(entt::registry& registry)
    {
        // Get the render world
        RenderWorld& renderWorld = _renderManager->GetRenderWorld();        

        // Upload scene data
        UploadGeometry(renderWorld);
        UploadSceneData(renderWorld);
    }

    void UploadRenderWorldSystem::UploadGeometry(RenderWorld& renderWorld)
    {
        // Get graphics device
        GraphicsDevice* graphicsDevice = _renderManager->GetGraphicsDevice();

        // Prepare render meshes
        size_t requiredUploadBufferSize = 0;
        for (auto& [ meshHandle, renderMesh ] : renderWorld.geometryCache)
        {
            // Get mesh and check dirty flag
            auto const mesh = meshHandle.Get();
            if (!mesh->GetDirtyFlag()) {
                continue;
            }

            // Get required buffer sizes
            size_t const vertexBufferSize = mesh->GetVertices().size() * sizeof(Vertex);
            size_t const indexBufferSize = mesh->GetIndices().size() * sizeof(uint32_t);

            // Set up render mesh buffer descriptions
            GraphicsBufferDesc vertexBufferDesc{};
            vertexBufferDesc.heapType = GraphicsHeapType_Default;
            vertexBufferDesc.size = vertexBufferSize;
            vertexBufferDesc.usage = GraphicsBufferUsage_VertexBuffer | GraphicsBufferUsage_TransferDst;

            GraphicsBufferDesc indexBufferDesc{};
            indexBufferDesc.heapType = GraphicsHeapType_Default;
            indexBufferDesc.size = indexBufferSize;
            indexBufferDesc.usage = GraphicsBufferUsage_IndexBuffer | GraphicsBufferUsage_TransferDst;

            // Recreate buffers
            graphicsDevice->DestroyBuffer(renderMesh.vertexBuffer);
            graphicsDevice->DestroyBuffer(renderMesh.indexBuffer);
            renderMesh.vertexBuffer = graphicsDevice->CreateBuffer(vertexBufferDesc);
            renderMesh.indexBuffer = graphicsDevice->CreateBuffer(indexBufferDesc);

            // Bump required upload buffer size
            requiredUploadBufferSize += vertexBufferSize + indexBufferSize;
        }

        // Check if upload is required
        if (requiredUploadBufferSize == 0) {
            return;
        }

        // Create upload buffer
        GraphicsBufferDesc uploadBufferDesc{};
        uploadBufferDesc.heapType = GraphicsHeapType_Upload;
        uploadBufferDesc.size = requiredUploadBufferSize;
        uploadBufferDesc.usage = GraphicsBufferUsage_TransferSrc;

        GraphicsBuffer* uploadBuffer = graphicsDevice->CreateBuffer(uploadBufferDesc);

        // Upload geometry in single transfer batch
        TransferBatch* transferBatch = graphicsDevice->StartTransferBatch();
        GraphicsCommandBuffer* transferCommandBuffer = graphicsDevice->BeginCommandBuffer();
        size_t uploadBufferPointer = 0;
        for (auto const& [ meshHandle, renderMesh ] : renderWorld.geometryCache)
        {
            // Get mesh and check dirty flag
            auto const mesh = meshHandle.Get();
            if (!mesh->GetDirtyFlag()) {
                continue;
            }

            // Copy data to upload buffer
            auto const& vertices = mesh->GetVertices();
            auto const& indices = mesh->GetIndices();
            size_t const vertexBufferSize = vertices.size() * sizeof(Vertex);
            size_t const indexBufferSize = indices.size() * sizeof(uint32_t);

            size_t const vertexBufferStart = uploadBufferPointer;
            graphicsDevice->CopyToBuffer(uploadBuffer, vertices.data(), vertexBufferSize, vertexBufferStart);
            uploadBufferPointer += vertexBufferSize;
            
            size_t const indexBufferStart = uploadBufferPointer;
            graphicsDevice->CopyToBuffer(uploadBuffer, indices.data(), indexBufferSize, indexBufferStart);
            uploadBufferPointer += indexBufferSize;

            // Copy data to device buffers
            transferCommandBuffer->CopyBufferToBuffer(uploadBuffer, renderMesh.vertexBuffer, vertexBufferSize, vertexBufferStart, 0);
            transferCommandBuffer->CopyBufferToBuffer(uploadBuffer, renderMesh.indexBuffer, indexBufferSize, indexBufferStart, 0);

            // Clear dirty flag
            mesh->SetDirtyFlag(false);
        }

        // Submit transfer batch
        graphicsDevice->SubmitTransferCommands(transferBatch, transferCommandBuffer);
        graphicsDevice->SubmitTransferBatch(transferBatch);

        // Perform cleanup
        graphicsDevice->WaitForTransferBatch(transferBatch);
        graphicsDevice->DestroyBuffer(uploadBuffer);
        SPDLOG_TRACE("Completed geometry upload ({} MiB | {} KiB | {} Bytes)", requiredUploadBufferSize >> 20, requiredUploadBufferSize >> 10, requiredUploadBufferSize);
    }

    void UploadRenderWorldSystem::UploadSceneData(RenderWorld& renderWorld)
    {
        // Get graphics device
        GraphicsDevice* graphicsDevice = _renderManager->GetGraphicsDevice();

        // Calculate shader buffer sizes
        size_t cameraBufferSize = renderWorld.cameras.size() * sizeof(Shader::Camera);
        size_t renderObjectBufferSize = renderWorld.renderObjects.size() * sizeof(Shader::RenderObject);
        size_t objectTransformBufferSize = renderWorld.objectTransforms.size() * sizeof(Shader::ObjectTransform);
        size_t subMeshBufferSize = renderWorld.subMeshes.size() * sizeof(Shader::SubMesh);
        size_t materialBufferSize = renderWorld.materials.size() * sizeof(Shader::Material);

        // Adjust sizes to allocate at least 1 entry per buffer to avoid empty buffers
        cameraBufferSize = std::max(cameraBufferSize, sizeof(Shader::Camera));
        renderObjectBufferSize = std::max(renderObjectBufferSize, sizeof(Shader::RenderObject));
        objectTransformBufferSize = std::max(objectTransformBufferSize, sizeof(Shader::ObjectTransform));
        subMeshBufferSize = std::max(subMeshBufferSize, sizeof(Shader::SubMesh));
        materialBufferSize = std::max(materialBufferSize, sizeof(Shader::Material));
        
        // Calculate required upload buffer size
        size_t const requiredUploadBufferSize = cameraBufferSize + renderObjectBufferSize + objectTransformBufferSize + subMeshBufferSize + materialBufferSize;

        // Set up render world buffers
        if (cameraBufferSize > renderWorld.cameraBuffer.size)
        {
            // Create camera buffer
            GraphicsBufferDesc cameraBufferDesc{};
            cameraBufferDesc.heapType = GraphicsHeapType_Default;
            cameraBufferDesc.size = cameraBufferSize;
            cameraBufferDesc.usage = GraphicsBufferUsage_StorageBuffer | GraphicsBufferUsage_TransferDst;

            graphicsDevice->DestroyBuffer(renderWorld.cameraBuffer.buffer);
            renderWorld.cameraBuffer.buffer = graphicsDevice->CreateBuffer(cameraBufferDesc);
            renderWorld.cameraBuffer.size = cameraBufferSize;
        }

        if (renderObjectBufferSize > renderWorld.renderObjectBuffer.size)
        {
            // Create render object buffer
            GraphicsBufferDesc renderObjectBufferDesc{};
            renderObjectBufferDesc.heapType = GraphicsHeapType_Default;
            renderObjectBufferDesc.size = renderObjectBufferSize;
            renderObjectBufferDesc.usage = GraphicsBufferUsage_StorageBuffer | GraphicsBufferUsage_TransferDst;

            graphicsDevice->DestroyBuffer(renderWorld.renderObjectBuffer.buffer);
            renderWorld.renderObjectBuffer.buffer = graphicsDevice->CreateBuffer(renderObjectBufferDesc);
            renderWorld.renderObjectBuffer.size = renderObjectBufferSize;
        }

        if (objectTransformBufferSize > renderWorld.objectTransformBuffer.size)
        {
            // Create object transform buffer
            GraphicsBufferDesc objectTransformBufferDesc{};
            objectTransformBufferDesc.heapType = GraphicsHeapType_Default;
            objectTransformBufferDesc.size = objectTransformBufferSize;
            objectTransformBufferDesc.usage = GraphicsBufferUsage_StorageBuffer | GraphicsBufferUsage_TransferDst;

            graphicsDevice->DestroyBuffer(renderWorld.objectTransformBuffer.buffer);
            renderWorld.objectTransformBuffer.buffer = graphicsDevice->CreateBuffer(objectTransformBufferDesc);
            renderWorld.objectTransformBuffer.size = objectTransformBufferSize;
        }

        if (subMeshBufferSize > renderWorld.subMeshBuffer.size)
        {
            // Create sub mesh buffer
            GraphicsBufferDesc subMeshBufferDesc{};
            subMeshBufferDesc.heapType = GraphicsHeapType_Default;
            subMeshBufferDesc.size = subMeshBufferSize;
            subMeshBufferDesc.usage = GraphicsBufferUsage_StorageBuffer | GraphicsBufferUsage_TransferDst;

            graphicsDevice->DestroyBuffer(renderWorld.subMeshBuffer.buffer);
            renderWorld.subMeshBuffer.buffer = graphicsDevice->CreateBuffer(subMeshBufferDesc);
            renderWorld.subMeshBuffer.size = subMeshBufferSize;
        }

        if (materialBufferSize > renderWorld.materialBuffer.size)
        {
            // Create sub mesh buffer
            GraphicsBufferDesc materialBufferDesc{};
            materialBufferDesc.heapType = GraphicsHeapType_Default;
            materialBufferDesc.size = materialBufferSize;
            materialBufferDesc.usage = GraphicsBufferUsage_StorageBuffer | GraphicsBufferUsage_TransferDst;

            graphicsDevice->DestroyBuffer(renderWorld.materialBuffer.buffer);
            renderWorld.materialBuffer.buffer = graphicsDevice->CreateBuffer(materialBufferDesc);
            renderWorld.materialBuffer.size = materialBufferSize;
        }

        // Possibly reallocate upload buffer
        if (requiredUploadBufferSize > _sceneDataUploadBufferSize)
        {
            // Calculate next step up for the upload buffer size
            size_t const uploadBufferSize = (requiredUploadBufferSize / UploadBufferIncrementSize) + UploadBufferIncrementSize;

            // Create upload buffer
            GraphicsBufferDesc uploadBufferDesc{};
            uploadBufferDesc.heapType = GraphicsHeapType_Upload;
            uploadBufferDesc.size = uploadBufferSize;
            uploadBufferDesc.usage = GraphicsBufferUsage_TransferSrc;

            graphicsDevice->DestroyBuffer(_sceneDataUploadBuffer);
            _sceneDataUploadBuffer = graphicsDevice->CreateBuffer(uploadBufferDesc);
            _sceneDataUploadBufferSize = uploadBufferSize;
        }

        // Upload scene data in single transfer batch
        TransferBatch* transferBatch = graphicsDevice->StartTransferBatch();
        GraphicsCommandBuffer* transferCommandBuffer = graphicsDevice->BeginCommandBuffer();
        size_t uploadBufferPointer = 0;

        // Copy camera data
        if (!renderWorld.cameras.empty())
        {
            graphicsDevice->CopyToBuffer(_sceneDataUploadBuffer, renderWorld.cameras.data(), cameraBufferSize, uploadBufferPointer);
            transferCommandBuffer->CopyBufferToBuffer(_sceneDataUploadBuffer, renderWorld.cameraBuffer.buffer, cameraBufferSize, uploadBufferPointer, 0);
            uploadBufferPointer += cameraBufferSize;
        }

        // Copy render object data
        if (!renderWorld.renderObjects.empty())
        {
            graphicsDevice->CopyToBuffer(_sceneDataUploadBuffer, renderWorld.renderObjects.data(), renderObjectBufferSize, uploadBufferPointer);
            transferCommandBuffer->CopyBufferToBuffer(_sceneDataUploadBuffer, renderWorld.renderObjectBuffer.buffer, renderObjectBufferSize, uploadBufferPointer, 0);
            uploadBufferPointer += renderObjectBufferSize;
        }

        // Copy object transform data
        if (!renderWorld.objectTransforms.empty())
        {
            graphicsDevice->CopyToBuffer(_sceneDataUploadBuffer, renderWorld.objectTransforms.data(), objectTransformBufferSize, uploadBufferPointer);
            transferCommandBuffer->CopyBufferToBuffer(_sceneDataUploadBuffer, renderWorld.objectTransformBuffer.buffer, objectTransformBufferSize, uploadBufferPointer, 0);
            uploadBufferPointer += objectTransformBufferSize;
        }

        // Copy sub mesh data
        if (!renderWorld.subMeshes.empty())
        {
            graphicsDevice->CopyToBuffer(_sceneDataUploadBuffer, renderWorld.subMeshes.data(), subMeshBufferSize, uploadBufferPointer);
            transferCommandBuffer->CopyBufferToBuffer(_sceneDataUploadBuffer, renderWorld.subMeshBuffer.buffer, subMeshBufferSize, uploadBufferPointer, 0);
            uploadBufferPointer += subMeshBufferSize;
        }

        // Copy material data
        if (!renderWorld.materials.empty())
        {
            graphicsDevice->CopyToBuffer(_sceneDataUploadBuffer, renderWorld.materials.data(), materialBufferSize, uploadBufferPointer);
            transferCommandBuffer->CopyBufferToBuffer(_sceneDataUploadBuffer, renderWorld.materialBuffer.buffer, materialBufferSize, uploadBufferPointer, 0);
            uploadBufferPointer += materialBufferSize;
        }

        // Submit transfer batch
        graphicsDevice->SubmitTransferCommands(transferBatch, transferCommandBuffer);
        graphicsDevice->SubmitTransferBatch(transferBatch);
        graphicsDevice->WaitForTransferBatch(transferBatch);
    }
} // namespace Bonsai
