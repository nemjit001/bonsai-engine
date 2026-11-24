#include "GeometryRenderer.hpp"

#include <Core/Logger.hpp>
#include <Gfx/GraphicsDevice.hpp>
#include "Renderer/RenderManager.hpp"
#include "Renderer/RenderWorld.hpp"
#include "Renderer/ShaderTypes.hpp"

namespace Bonsai
{
    bool GeometryRenderer::Initialize(RenderManager* renderManager, ShaderCompiler* shaderCompiler)
    {
        SPDLOG_TRACE("Creating geometry renderer pipelines");
        _renderManager = renderManager;
        assert(_renderManager != nullptr);

        // Get graphics device
        GraphicsDevice* graphicsDevice = _renderManager->GetGraphicsDevice();

        // Create scene data descriptor set layout
        GraphicsDescriptorSetLayoutBinding sceneDataDescriptorBindings[] = {
            GraphicsDescriptorSetLayoutBinding{ 0, GraphicsDescriptorType_StorageBuffer, 1, GraphicsShaderStage_Vertex, nullptr },
            GraphicsDescriptorSetLayoutBinding{ 1, GraphicsDescriptorType_StorageBuffer, 1, GraphicsShaderStage_Vertex, nullptr },
            GraphicsDescriptorSetLayoutBinding{ 2, GraphicsDescriptorType_StorageBuffer, 1, GraphicsShaderStage_Vertex, nullptr },
        };

        GraphicsDescriptorSetLayoutDesc sceneDataDescriptorSetLayoutDesc{};
        sceneDataDescriptorSetLayoutDesc.bindingCount = std::size(sceneDataDescriptorBindings);
        sceneDataDescriptorSetLayoutDesc.bindings = sceneDataDescriptorBindings;

        _sceneDataDescriptorSetLayout = graphicsDevice->CreateDescriptorSetLayout(sceneDataDescriptorSetLayoutDesc);
        if (_sceneDataDescriptorSetLayout == nullptr) {
            return false;
        }

        // Set up render pipeline layout
        GraphicsPushConstantRange pushConstantRanges[] = {
            GraphicsPushConstantRange{ GraphicsShaderStage_Vertex, 0, sizeof(Shader::DrawParams) },
        };

        GraphicsPipelineLayoutDesc renderPipelineLayoutDesc{};
        renderPipelineLayoutDesc.descriptorSetLayoutCount = 1;
        renderPipelineLayoutDesc.descriptorSetLayouts = &_sceneDataDescriptorSetLayout;
        renderPipelineLayoutDesc.pushConstantRangeCount = std::size(pushConstantRanges);
        renderPipelineLayoutDesc.pushConstantRanges = pushConstantRanges;

        _renderPipelineLayout = graphicsDevice->CreatePipelineLayout(renderPipelineLayoutDesc);
        if (_renderPipelineLayout == nullptr) {
            return false;
        }

        // Compile unlit pipeline shader
        Slang::ComPtr<slang::IBlob> const unlitShader = shaderCompiler->CompileShaderFromFile(Filesystem::GetResourcePath("resources/shaders/unlit.slang"));
        if (unlitShader == nullptr) {
            return false;
        }

        // Set up vertex input state
        GraphicsVertexBinding vertexBindings[] = {
            GraphicsVertexBinding{ 0, sizeof(Vertex), GraphicsVertexInputRate_PerVertex },
        };

        GraphicsVertexAttribute vertexAttributes[] = {
            GraphicsVertexAttribute{ 0, 0, GraphicsFormat_RGB32_SFloat, offsetof(Vertex, position) },
            GraphicsVertexAttribute{ 1, 0, GraphicsFormat_RGB32_SFloat, offsetof(Vertex, normal) },
            GraphicsVertexAttribute{ 2, 0, GraphicsFormat_RGB32_SFloat, offsetof(Vertex, tangent) },
            GraphicsVertexAttribute{ 3, 0, GraphicsFormat_RG32_SFloat, offsetof(Vertex, texCoords) },
        };

        // Create unlit render pipeline
        GraphicsRenderPipelineDesc unlitPipelineDesc{};
        unlitPipelineDesc.pipelineLayout = _renderPipelineLayout;
        unlitPipelineDesc.vertexShaderEntrypoint = "VSUnlitMain"_entrypoint;
        unlitPipelineDesc.fragmentShaderEntrypoint = "PSUnlitMain"_entrypoint;
        unlitPipelineDesc.vertexShaderBytecode = GraphicsShaderBytecode{ unlitShader->getBufferPointer(), unlitShader->getBufferSize() };
        unlitPipelineDesc.fragmentShaderBytecode = GraphicsShaderBytecode{ unlitShader->getBufferPointer(), unlitShader->getBufferSize() };
        unlitPipelineDesc.vertexInputState.vertexBindingCount = std::size(vertexBindings);
        unlitPipelineDesc.vertexInputState.vertexBindings = vertexBindings;
        unlitPipelineDesc.vertexInputState.vertexAttributeCount = std::size(vertexAttributes);
        unlitPipelineDesc.vertexInputState.vertexAttributes = vertexAttributes;
        unlitPipelineDesc.primitiveTopology = GraphicsPrimitiveTopology_TriangleList;
        unlitPipelineDesc.stripCutValue = GraphicsIndexBufferStripCutValue_None;
        unlitPipelineDesc.rasterState.polygonMode = GraphicsPolygonMode_Fill;
        unlitPipelineDesc.rasterState.cullMode = GraphicsCullMode_None;
        unlitPipelineDesc.rasterState.frontCounterClockwise = false;
        unlitPipelineDesc.multisampleState.sampleCount = 1;
        unlitPipelineDesc.depthStencilState.depthTest = true;
        unlitPipelineDesc.depthStencilState.depthWrite = true;
        unlitPipelineDesc.depthStencilState.depthCompareOp = GraphicsCompareOp_Less;
        unlitPipelineDesc.blendState.renderTargets[0].blendEnable = false;
        unlitPipelineDesc.blendState.renderTargets[0].colorWriteMask = GraphicsColorComponent_All;
        unlitPipelineDesc.renderTargetCount = 1;
        unlitPipelineDesc.renderTargetFormats[0] = GraphicsFormat_RGBA16_SFloat;
        unlitPipelineDesc.depthStencilFormat = GraphicsFormat_D24_UNorm_S8_UInt;

        _unlitRenderPipeline = graphicsDevice->CreateRenderPipeline(unlitPipelineDesc);
        if (_unlitRenderPipeline == nullptr) {
            return false;
        }

        return true;
    }

    void GeometryRenderer::Shutdown()
    {
        GraphicsDevice* graphicsDevice = _renderManager->GetGraphicsDevice();
        graphicsDevice->DestroyRenderPipeline(_unlitRenderPipeline);
        graphicsDevice->DestroyPipelineLayout(_renderPipelineLayout);
        graphicsDevice->DestroyDescriptorSetLayout(_sceneDataDescriptorSetLayout);
    }

    void GeometryRenderer::PrepareRender(FrameContext* frameContext, RenderWorld const& renderWorld)
    {
        // Get graphics device
        GraphicsDevice* graphicsDevice = _renderManager->GetGraphicsDevice();

        // Create scene descriptor set
        GraphicsDescriptorBufferInfo cameraBufferInfo{};
        cameraBufferInfo.buffer = renderWorld.cameraBuffer;
        cameraBufferInfo.offset = 0;
        cameraBufferInfo.range = renderWorld.cameraBufferSize;

        GraphicsDescriptorBufferInfo renderObjectBufferInfo{};
        renderObjectBufferInfo.buffer = renderWorld.renderObjectBuffer;
        renderObjectBufferInfo.offset = 0;
        renderObjectBufferInfo.range =  renderWorld.renderObjectBufferSize;

        GraphicsDescriptorBufferInfo objectTransformBufferInfo{};
        objectTransformBufferInfo.buffer = renderWorld.objectTransformBuffer;
        objectTransformBufferInfo.offset = 0;
        objectTransformBufferInfo.range =  renderWorld.objectTransformBufferSize;

        GraphicsDescriptorSetBinding sceneDescriptorSetBindings[] = {
            GraphicsDescriptorSetBinding{ 0, 0, 1, GraphicsDescriptorType_StorageBuffer, &cameraBufferInfo, nullptr },
            GraphicsDescriptorSetBinding{ 1, 0, 1, GraphicsDescriptorType_StorageBuffer, &renderObjectBufferInfo, nullptr },
            GraphicsDescriptorSetBinding{ 2, 0, 1, GraphicsDescriptorType_StorageBuffer, &objectTransformBufferInfo, nullptr },
        };

        GraphicsDescriptorSetDesc sceneDescriptorSetDesc{};
        sceneDescriptorSetDesc.descriptorSetLayout = _sceneDataDescriptorSetLayout;
        sceneDescriptorSetDesc.bindingCount = std::size(sceneDescriptorSetBindings);
        sceneDescriptorSetDesc.bindings = sceneDescriptorSetBindings;

        _sceneDataDescriptorSet = graphicsDevice->CreateDescriptorSet(frameContext, sceneDescriptorSetDesc);
        if (_sceneDataDescriptorSet == nullptr) {
            SPDLOG_WARN("Failed to create scene descriptor set");
        }
    }

    void GeometryRenderer::Render(FrameContext* frameContext, Extent2D const& renderExtent, RenderWorld const& renderWorld)
    {
        // Get graphics device and render targets
        GraphicsDevice* graphicsDevice = _renderManager->GetGraphicsDevice();
        RenderTarget const& colorTarget = _renderManager->GetColorTarget();
        RenderTarget const& depthStencilTarget = _renderManager->GetDepthStencilTarget();

        // Get command buffer
        GraphicsCommandBuffer* commandBuffer = graphicsDevice->BeginCommandBuffer();

        // Prepare render targets for write
        {
            GraphicsTextureBarrier colorTargetWriteBarrier{};
            colorTargetWriteBarrier.texture = colorTarget.target;
            colorTargetWriteBarrier.srcLayout = GraphicsTextureLayout_Undefined;
            colorTargetWriteBarrier.srcResourceState = GraphicsResourceState_RenderTarget_Read;
            colorTargetWriteBarrier.dstLayout = GraphicsTextureLayout_RenderTarget;
            colorTargetWriteBarrier.dstResourceState = GraphicsResourceState_RenderTarget_Write;
            colorTargetWriteBarrier.subresource.aspectMask = GraphicsTextureAspect_Color;
            colorTargetWriteBarrier.subresource.baseMipLevel = 0;
            colorTargetWriteBarrier.subresource.levelCount = 1;
            colorTargetWriteBarrier.subresource.baseArrayLayer = 0;
            colorTargetWriteBarrier.subresource.layerCount = 1;

            GraphicsTextureBarrier depthStencilTargetWriteBarrier{};
            depthStencilTargetWriteBarrier.texture = depthStencilTarget.target;
            depthStencilTargetWriteBarrier.srcLayout = GraphicsTextureLayout_Undefined;
            depthStencilTargetWriteBarrier.srcResourceState = GraphicsResourceState_DepthStencilTarget_Read;
            depthStencilTargetWriteBarrier.dstLayout = GraphicsTextureLayout_DepthStencilTarget;
            depthStencilTargetWriteBarrier.dstResourceState = GraphicsResourceState_DepthStencilTarget_Write;
            depthStencilTargetWriteBarrier.subresource.aspectMask = GraphicsTextureAspect_Depth | GraphicsTextureAspect_Stencil;
            depthStencilTargetWriteBarrier.subresource.baseMipLevel = 0;
            depthStencilTargetWriteBarrier.subresource.levelCount = 1;
            depthStencilTargetWriteBarrier.subresource.baseArrayLayer = 0;
            depthStencilTargetWriteBarrier.subresource.layerCount = 1;

            GraphicsTextureBarrier renderAttachmentBarriers[] = { colorTargetWriteBarrier, depthStencilTargetWriteBarrier };
            commandBuffer->PipelineBarrier(nullptr, 0, nullptr, 0, renderAttachmentBarriers, std::size(renderAttachmentBarriers));
        }

        // Unlit render pass
        {
            // Begin render pass
            GraphicsRenderAttachmentInfo colorAttachmentInfo{};
            colorAttachmentInfo.renderTarget = colorTarget.targetView;
            colorAttachmentInfo.loadOp = GraphicsLoadOp_Clear;
            colorAttachmentInfo.storeOp = GraphicsStoreOp_Store;
            colorAttachmentInfo.clearValue.color = GraphicsClearColorValue{{ 0.25F, 0.25F, 0.25F, 0.25F }};

            GraphicsRenderAttachmentInfo depthStencilAttachmentInfo{};
            depthStencilAttachmentInfo.renderTarget = depthStencilTarget.targetView;
            depthStencilAttachmentInfo.loadOp = GraphicsLoadOp_Clear;
            depthStencilAttachmentInfo.storeOp = GraphicsStoreOp_DontCare;
            depthStencilAttachmentInfo.clearValue.depthStencil = GraphicsClearDepthStencilValue{ 1.0F, 0u };

            Rect2D renderArea{};
            renderArea.offset = Offset2D{ 0, 0 };
            renderArea.extent = renderExtent;

            commandBuffer->BeginRendering(renderArea, &colorAttachmentInfo, 1, &depthStencilAttachmentInfo);

            // Set viewport and scissor
            GraphicsViewport viewport{};
            viewport.x = 0.0F;
            viewport.y = 0.0F;
            viewport.width = static_cast<float>(renderExtent.width);
            viewport.height = static_cast<float>(renderExtent.height);
            viewport.minDepth = 0.0F;
            viewport.maxDepth = 1.0F;
            
            Rect2D scissorRect{};
            scissorRect.offset = Offset2D{ 0, 0 };
            scissorRect.extent = renderExtent;

            commandBuffer->SetViewport(viewport);
            commandBuffer->SetScissorRect(scissorRect);

            // Bind pipeline state
            commandBuffer->BindRenderPipeline(_unlitRenderPipeline);
            commandBuffer->BindRenderDescriptorSets(_renderPipelineLayout, 0, &_sceneDataDescriptorSet, 1);

            // Record draw commands
            uint32_t lastObjectId = UINT32_MAX;
            for (auto const& drawCommand : renderWorld.geometryRenderQueue)
            {
                // Get draw params
                uint32_t const cameraId = drawCommand.sortKey.fields.cameraId;
                uint32_t const objectId = drawCommand.sortKey.fields.objectId;
                uint32_t const meshId = drawCommand.sortKey.fields.meshId;

                // Validate render mesh
                if (drawCommand.renderMesh.vertexBuffer == nullptr
                    || drawCommand.renderMesh.indexBuffer == nullptr
                )
                {
                    SPDLOG_WARN("Invalid render mesh encountered for draw {}:{}:{}", cameraId, objectId, meshId);
                    continue;
                }

                // Push draw params
                Shader::DrawParams drawParams{};
                drawParams.cameraId = cameraId;
                drawParams.objectId = objectId;
                drawParams.meshId = meshId;

                commandBuffer->PushConstants(_renderPipelineLayout, GraphicsShaderStage_Vertex, 0, sizeof(drawParams), &drawParams);

                // Bind new render mesh buffers when object changes
                if (objectId != lastObjectId)
                {
                    size_t offsets[] = { 0 };
                    commandBuffer->BindVertexBuffers(0, &drawCommand.renderMesh.vertexBuffer, 1, offsets);
                    commandBuffer->BindIndexBuffer(drawCommand.renderMesh.indexBuffer, 0, GraphicsIndexType_UInt32);

                    lastObjectId = objectId;
                }

                // Draw submesh
                commandBuffer->DrawIndexed(drawCommand.subMesh.indexCount, 1, drawCommand.subMesh.indexOffset, drawCommand.subMesh.vertexOffset, 0);
            }

            // End render pass
            commandBuffer->EndRendering();
        }

        // Submit commands
        graphicsDevice->SubmitFrameCommands(frameContext, commandBuffer);
    }

    void GeometryRenderer::PostRender()
    {
        GraphicsDevice* graphicsDevice = _renderManager->GetGraphicsDevice();
        graphicsDevice->DestroyDescriptorSet(_sceneDataDescriptorSet);
    }
} // namespace Bonsai
