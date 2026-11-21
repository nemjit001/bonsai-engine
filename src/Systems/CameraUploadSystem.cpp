#include "CameraUploadSystem.hpp"

#include <cassert>
#include "Renderer/ShaderTypes.hpp"
#include "Components/CameraComponent.hpp"
#include "Components/TransformComponent.hpp"

namespace Bonsai
{
    CameraUploadSystem::CameraUploadSystem(RenderManager* renderManager)
    :
        _renderManager(renderManager)
    {
        assert(_renderManager != nullptr && "renderManager was nullptr!");
    }

    void CameraUploadSystem::Run(entt::registry& registry)
    {
        // Get the graphics device
        GraphicsDevice* graphicsDevice = _renderManager->GetGraphicsDevice();
        assert(graphicsDevice != nullptr && "graphicsDevice was nullptr!");

        // Get the view into the registry
        auto view = registry.view<TransformComponent, CameraComponent>();

        // Create buffer for each camera
        for (auto const& [ _, transformComponent, cameraComponent ] : view.each())
        {
            // Check if camera component already has a buffer
            if (cameraComponent.cameraConstantBuffer != nullptr) {
                continue;
            }

            // Create camera constant buffer
            GraphicsBufferDesc cameraConstantBufferDesc{};
            cameraConstantBufferDesc.heapType = GraphicsHeapType_Upload; // Use an upload heap, since this will be updated every frame
            cameraConstantBufferDesc.size = sizeof(Shader::Camera);
            cameraConstantBufferDesc.usage = GraphicsBufferUsage_UniformBuffer;

            graphicsDevice->DestroyBuffer(cameraComponent.cameraConstantBuffer);
            cameraComponent.cameraConstantBuffer = graphicsDevice->CreateBuffer(cameraConstantBufferDesc);
        }

        // Copy camera data
        for (auto const& [ _, transformComponent, cameraComponent ] : view.each())
        {
            // Get camera matrices
            glm::mat4 const viewMatrix = glm::inverse(transformComponent.GetMatrix());
            glm::mat4 const projectMatrix = cameraComponent.camera.GetProjectionMatrix();

            // Copy camera data to buffer
            Shader::Camera cameraData{};
            cameraData.viewProjectMatrix = projectMatrix * viewMatrix;

            graphicsDevice->CopyToBuffer(cameraComponent.cameraConstantBuffer, &cameraData, sizeof(cameraData), 0);
        }
    }
} // namespace Bonsai
