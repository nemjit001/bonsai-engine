#include "RenderWorldSyncSystem.hpp"

#include <cassert>
#include <Core/Logger.hpp>
#include "Renderer/RenderManager.hpp"
#include "Components/CameraComponent.hpp"
#include "Components/MeshComponent.hpp"
#include "Components/TransformComponent.hpp"

namespace Bonsai
{
    RenderWorldSyncSystem::RenderWorldSyncSystem(RenderManager* renderManager)
        :
        _renderManager(renderManager)
    {
        assert(_renderManager != nullptr && "renderManager was null!");
    }

    RenderWorldSyncSystem::~RenderWorldSyncSystem()
    {
        // Get graphics device
        GraphicsDevice* graphicsDevice = _renderManager->GetGraphicsDevice();
        graphicsDevice->DestroyBuffer(_sceneDataUploadBuffer);
    }

    void RenderWorldSyncSystem::Run(entt::registry& registry)
    {
        // Get the render world
        RenderWorld& renderWorld = _renderManager->GetRenderWorld();

        // Update host side render world data
        UpdateRenderObjectCache(renderWorld, registry);
        UpdateGeometryCache(renderWorld, registry);
        UpdateCameraData(renderWorld, registry);
        UpdateRenderObjectData(renderWorld, registry);

        // Upload data
        UploadGeometry(renderWorld);
        UploadSceneData(renderWorld);
    }

    void RenderWorldSyncSystem::UpdateRenderObjectCache(RenderWorld& renderWorld, entt::registry const& registry)
    {
        // Get relevant ECS view
        auto const renderObjectView = registry.view<TransformComponent, MeshComponent>();

        // Mark unused render objects as free, also erasing them from the object cache
        std::vector<entt::entity> eraseList;
        for (auto const& [ entity, renderObjectId ] : renderWorld.renderObjectCache)
        {
            // Check if the object still exists in the registry view
            if (renderObjectView.contains(entity)) {
                continue;
            }

            // Mark objects that no longer exist as free
            renderWorld.renderObjectFreeList.push_back(renderObjectId);
            eraseList.push_back(entity);
            SPDLOG_TRACE("Freed unused render object (entity: {} objectId: {})", entt::to_integral(entity), renderObjectId);
        }

        for (auto const& entity : eraseList) {
            renderWorld.renderObjectCache.erase(entity);
        }

        // Update render objects cache
        for (auto const& [ entity, transformComponent, meshComponent ] : renderObjectView.each())
        {
            // Check if render object already has an ID
            auto const iter = renderWorld.renderObjectCache.find(entity);
            if (iter != renderWorld.renderObjectCache.end()) {
                continue;
            }

            // Assign either a free object ID, or allocate a new render object ID
            RenderObjectId renderObjectId = 0;
            if (!renderWorld.renderObjectFreeList.empty())
            {
                renderObjectId = renderWorld.renderObjectFreeList.back();
                renderWorld.renderObjectFreeList.pop_back();
            }
            else
            {
                renderObjectId = renderWorld.maxRenderObjectId;
                renderWorld.maxRenderObjectId++;
            }

            // Insert new render object
            renderWorld.renderObjectCache.insert({ entity, renderObjectId });
            SPDLOG_TRACE("Inserted new render object (entity: {} objectId: {})", entt::to_integral(entity), renderObjectId);
        }
    }

    void RenderWorldSyncSystem::UpdateGeometryCache(RenderWorld& renderWorld, entt::registry const& registry)
    {
        // Get relevant ECS view
        auto const meshView = registry.view<MeshComponent>();

        // Insert new geometry into geometry cache
        for (auto const& [ _, meshComponent ] : meshView.each())
        {
            // Check if mesh has already been inserted into the geometry cache
            auto const iter = renderWorld.geometryCache.find(meshComponent.meshHandle);
            if (iter != renderWorld.geometryCache.end()) {
                continue;
            }

            // Insert new render mesh into the geometry cache
            renderWorld.geometryCache.insert({ meshComponent.meshHandle, RenderMesh{} });
            SPDLOG_TRACE("Inserted new mesh into geometry cache (asset id: {})", meshComponent.meshHandle.GetAssetId());
        }
    }

    void RenderWorldSyncSystem::UpdateCameraData(RenderWorld& renderWorld, entt::registry const& registry)
    {
        // Get relevant ECS view
        auto const cameraView = registry.view<TransformComponent, CameraComponent>();

        // Update cameras in the scene
        renderWorld.cameras.clear();
        for (auto const& [ entity, transformComponent, cameraComponent ] : cameraView.each())
        {
            // Get camera matrices
            glm::mat4 const viewMatrix = glm::inverse(transformComponent.GetMatrix());
            glm::mat4 const projectionMatrix = cameraComponent.camera.GetProjectionMatrix();

            // Set up camera data
            Shader::Camera camera{};
            camera.viewProjectMatrix = projectionMatrix * viewMatrix;
            renderWorld.cameras.push_back(camera);
        }
    }

    void RenderWorldSyncSystem::UpdateRenderObjectData(RenderWorld& renderWorld, entt::registry const& registry)
    {
        // Get relevant ECS view
        auto const renderObjectView = registry.view<TransformComponent, MeshComponent>();

        // Resize and update render object buffers
        renderWorld.renderObjects.resize(renderWorld.maxRenderObjectId);
        renderWorld.objectTransforms.resize(renderWorld.maxRenderObjectId);
        for (auto const& [ entity, transformComponent, meshComponent ] : renderObjectView.each())
        {
            // Get render object ID from cache
            RenderObjectId const& objectId = renderWorld.renderObjectCache.at(entity);

            // Update render object data
            Shader::RenderObject& renderObject = renderWorld.renderObjects[objectId];
            renderObject.objectId = objectId;

            // Update object transform
            Shader::ObjectTransform& objectTransform = renderWorld.objectTransforms[objectId];
            objectTransform.modelMatrix = transformComponent.GetMatrix();
            objectTransform.normalMatrix = glm::inverse(glm::transpose(glm::mat3(objectTransform.modelMatrix)));
        }
    }

    void RenderWorldSyncSystem::UploadGeometry(RenderWorld& renderWorld)
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

    void RenderWorldSyncSystem::UploadSceneData(RenderWorld& renderWorld)
    {
        // Get graphics device
        GraphicsDevice* graphicsDevice = _renderManager->GetGraphicsDevice();

        // Calculate shader buffer sizes (allocate 1 more to avoid empty buffers)
        size_t const cameraBufferSize = (renderWorld.cameras.size() + 1) * sizeof(Shader::Camera);
        size_t const renderObjectBufferSize = (renderWorld.renderObjects.size() + 1) * sizeof(Shader::RenderObject);
        size_t const objectTransformBufferSize = (renderWorld.objectTransforms.size() + 1) * sizeof(Shader::ObjectTransform);
        size_t const requiredUploadBufferSize = cameraBufferSize + renderObjectBufferSize + objectTransformBufferSize;

        // Set up render world buffers
        if (cameraBufferSize > renderWorld.cameraBufferSize)
        {
            // Create camera buffer
            GraphicsBufferDesc cameraBufferDesc{};
            cameraBufferDesc.heapType = GraphicsHeapType_Default;
            cameraBufferDesc.size = cameraBufferSize;
            cameraBufferDesc.usage = GraphicsBufferUsage_StorageBuffer | GraphicsBufferUsage_TransferDst;

            graphicsDevice->DestroyBuffer(renderWorld.cameraBuffer);
            renderWorld.cameraBuffer = graphicsDevice->CreateBuffer(cameraBufferDesc);
            renderWorld.cameraBufferSize = cameraBufferSize;
        }

        if (renderObjectBufferSize > renderWorld.renderObjectBufferSize)
        {
            // Create render object buffer
            GraphicsBufferDesc renderObjectBufferDesc{};
            renderObjectBufferDesc.heapType = GraphicsHeapType_Default;
            renderObjectBufferDesc.size = renderObjectBufferSize;
            renderObjectBufferDesc.usage = GraphicsBufferUsage_StorageBuffer | GraphicsBufferUsage_TransferDst;

            graphicsDevice->DestroyBuffer(renderWorld.renderObjectBuffer);
            renderWorld.renderObjectBuffer = graphicsDevice->CreateBuffer(renderObjectBufferDesc);
            renderWorld.renderObjectBufferSize = renderObjectBufferSize;
        }

        if (objectTransformBufferSize > renderWorld.objectTransformBufferSize)
        {
            // Create object transform buffer
            GraphicsBufferDesc objectTransformBufferDesc{};
            objectTransformBufferDesc.heapType = GraphicsHeapType_Default;
            objectTransformBufferDesc.size = objectTransformBufferSize;
            objectTransformBufferDesc.usage = GraphicsBufferUsage_StorageBuffer | GraphicsBufferUsage_TransferDst;

            graphicsDevice->DestroyBuffer(renderWorld.objectTransformBuffer);
            renderWorld.objectTransformBuffer = graphicsDevice->CreateBuffer(objectTransformBufferDesc);
            renderWorld.objectTransformBufferSize = objectTransformBufferSize;
        }

        // Check if upload is required
        if (requiredUploadBufferSize == 0) {
            return;
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
        size_t const cameraBufferCopySize = renderWorld.cameras.size() * sizeof(Shader::Camera);
        if (renderWorld.cameraBuffer != nullptr && cameraBufferCopySize != 0)
        {
            graphicsDevice->CopyToBuffer(_sceneDataUploadBuffer, renderWorld.cameras.data(), cameraBufferCopySize, uploadBufferPointer);
            transferCommandBuffer->CopyBufferToBuffer(_sceneDataUploadBuffer, renderWorld.cameraBuffer, cameraBufferCopySize, uploadBufferPointer, 0);
            uploadBufferPointer += cameraBufferCopySize;
        }

        // Copy render object data
        size_t const renderObjectBufferCopySize = renderWorld.renderObjects.size() * sizeof(Shader::RenderObject);
        if (renderWorld.renderObjectBuffer != nullptr && renderObjectBufferCopySize != 0)
        {
            graphicsDevice->CopyToBuffer(_sceneDataUploadBuffer, renderWorld.renderObjects.data(), renderObjectBufferCopySize, uploadBufferPointer);
            transferCommandBuffer->CopyBufferToBuffer(_sceneDataUploadBuffer, renderWorld.renderObjectBuffer, renderObjectBufferCopySize, uploadBufferPointer, 0);
            uploadBufferPointer += renderObjectBufferCopySize;
        }

        // Copy object transform data
        size_t const objectTransformBufferCopySize = renderWorld.objectTransforms.size() * sizeof(Shader::ObjectTransform);
        if (renderWorld.objectTransformBuffer != nullptr && objectTransformBufferCopySize != 0)
        {
            graphicsDevice->CopyToBuffer(_sceneDataUploadBuffer, renderWorld.objectTransforms.data(), objectTransformBufferCopySize, uploadBufferPointer);
            transferCommandBuffer->CopyBufferToBuffer(_sceneDataUploadBuffer, renderWorld.objectTransformBuffer, objectTransformBufferCopySize, uploadBufferPointer, 0);
            uploadBufferPointer += objectTransformBufferCopySize;
        }

        // Submit transfer batch
        graphicsDevice->SubmitTransferCommands(transferBatch, transferCommandBuffer);
        graphicsDevice->SubmitTransferBatch(transferBatch);
        graphicsDevice->WaitForTransferBatch(transferBatch);
    }
} // namespace Bonsai
