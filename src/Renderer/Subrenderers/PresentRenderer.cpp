#include "PresentRenderer.hpp"

#include <Core/Logger.hpp>
#include <Gfx/GraphicsDevice.hpp>
#include "Renderer/RenderManager.hpp"

namespace Bonsai
{
    bool PresentRenderer::Initialize(RenderManager* renderManager, ShaderCompiler* shaderCompiler)
    {
        SPDLOG_TRACE("Creating present renderer pipelines");
        _renderManager = renderManager;
        assert(_renderManager != nullptr);
        
        // Get graphics device and swapchain
        GraphicsDevice* graphicsDevice = _renderManager->GetGraphicsDevice();
        GraphicsSwapchain* swapchain = _renderManager->GetSwapchain();

        // Create fullscreen sampler
        GraphicsSamplerDesc fullscreenSamplerDesc{};
        fullscreenSamplerDesc.magFilter = GraphicsFilter_Linear;
        fullscreenSamplerDesc.minFilter = GraphicsFilter_Linear;
        fullscreenSamplerDesc.mipmapMode = GraphicsSamplerMipmapMode_Linear;

        _fullscreenSampler = graphicsDevice->CreateSampler(fullscreenSamplerDesc);
        if (!_fullscreenSampler) {
            return false;
        }

        // Create present descriptor set layout
        GraphicsDescriptorSetLayoutBinding presentDescriptorBindings[] = {
            GraphicsDescriptorSetLayoutBinding{ 0, GraphicsDescriptorType_CombinedImageSampler, 1, GraphicsShaderStage_Fragment, nullptr },
        };

        GraphicsDescriptorSetLayoutDesc presentDescriptorSetLayoutDesc{};
        presentDescriptorSetLayoutDesc.bindingCount = std::size(presentDescriptorBindings);
        presentDescriptorSetLayoutDesc.bindings = presentDescriptorBindings;

        _presentDescriptorSetLayout = graphicsDevice->CreateDescriptorSetLayout(presentDescriptorSetLayoutDesc);
        if (_presentDescriptorSetLayout == nullptr) {
            return false;
        }

        // Set up present pipeline layout
        GraphicsPipelineLayoutDesc presentPipelineLayoutDesc{};
        presentPipelineLayoutDesc.descriptorSetLayoutCount = 1;
        presentPipelineLayoutDesc.descriptorSetLayouts = &_presentDescriptorSetLayout;
        presentPipelineLayoutDesc.pushConstantRangeCount = 0;
        presentPipelineLayoutDesc.pushConstantRanges = nullptr;

        _presentPipelineLayout = graphicsDevice->CreatePipelineLayout(presentPipelineLayoutDesc);
        if (_presentPipelineLayout == nullptr) {
            return false;
        }

        // Compile present pipeline shader
        Slang::ComPtr<slang::IBlob> const presentShader = shaderCompiler->CompileShaderFromFile(Filesystem::GetResourcePath("resources/shaders/fullscreen_quad.slang"));
        if (presentShader == nullptr) {
            return false;
        }

        // Create present render pipeline
        GraphicsRenderPipelineDesc presentPipelineDesc{};
        presentPipelineDesc.pipelineLayout = _presentPipelineLayout;
        presentPipelineDesc.vertexShaderEntrypoint = "VSFullscreenQuadMain"_entrypoint;
        presentPipelineDesc.fragmentShaderEntrypoint = "PSFullscreenQuadMain"_entrypoint;
        presentPipelineDesc.vertexShaderBytecode = GraphicsShaderBytecode{ presentShader->getBufferPointer(), presentShader->getBufferSize() };
        presentPipelineDesc.fragmentShaderBytecode = GraphicsShaderBytecode{ presentShader->getBufferPointer(), presentShader->getBufferSize() };
        presentPipelineDesc.primitiveTopology = GraphicsPrimitiveTopology_TriangleList;
        presentPipelineDesc.stripCutValue = GraphicsIndexBufferStripCutValue_None;
        presentPipelineDesc.rasterState.polygonMode = GraphicsPolygonMode_Fill;
        presentPipelineDesc.rasterState.cullMode = GraphicsCullMode_None;
        presentPipelineDesc.rasterState.frontCounterClockwise = false;
        presentPipelineDesc.multisampleState.sampleCount = 1;
        presentPipelineDesc.blendState.renderTargets[0].blendEnable = false;
        presentPipelineDesc.blendState.renderTargets[0].colorWriteMask = GraphicsColorComponent_All;
        presentPipelineDesc.renderTargetCount = 1;
        presentPipelineDesc.renderTargetFormats[0] = graphicsDevice->GetSwapchainFormat(swapchain);
        presentPipelineDesc.depthStencilFormat = GraphicsFormat_Unknown;

        _presentRenderPipeline = graphicsDevice->CreateRenderPipeline(presentPipelineDesc);
        if (_presentRenderPipeline == nullptr) {
            return false;
        }

        return true;
    }

    void PresentRenderer::Shutdown()
    {
        GraphicsDevice* graphicsDevice = _renderManager->GetGraphicsDevice();
        graphicsDevice->DestroyRenderPipeline(_presentRenderPipeline);
        graphicsDevice->DestroyPipelineLayout(_presentPipelineLayout);
        graphicsDevice->DestroyDescriptorSetLayout(_presentDescriptorSetLayout);
        graphicsDevice->DestroySampler(_fullscreenSampler);
    }

    void PresentRenderer::PrepareRender(FrameContext* frameContext, RenderWorld const&)
    {
        // Get graphics device and color target
        GraphicsDevice* graphicsDevice = _renderManager->GetGraphicsDevice();
        RenderTarget const& colorTarget = _renderManager->GetColorTarget();

        // Create present descriptor set
        GraphicsDescriptorImageInfo screenTextureImageInfo{};
        screenTextureImageInfo.sampler = _fullscreenSampler;
        screenTextureImageInfo.textureView = colorTarget.targetView;
        screenTextureImageInfo.textureLayout = GraphicsTextureLayout_ShaderReadOnly;

        GraphicsDescriptorSetBinding presentDescriptorSetBindings[] = {
            GraphicsDescriptorSetBinding{ 0, 0, 1, GraphicsDescriptorType_CombinedImageSampler, nullptr, &screenTextureImageInfo },
        };

        GraphicsDescriptorSetDesc presentDescriptorSetDesc{};
        presentDescriptorSetDesc.descriptorSetLayout = _presentDescriptorSetLayout;
        presentDescriptorSetDesc.bindingCount = std::size(presentDescriptorSetBindings);
        presentDescriptorSetDesc.bindings = presentDescriptorSetBindings;

        _presentDescriptorSet = graphicsDevice->CreateDescriptorSet(frameContext, presentDescriptorSetDesc);
        if (_presentDescriptorSet == nullptr) {
            SPDLOG_WARN("Failed to create present descriptor set");
        }
    }

    void PresentRenderer::Render(FrameContext* frameContext, Extent2D const& renderExtent, RenderWorld const& renderWorld)
    {
        // Get graphics device and swapchain
        GraphicsDevice* graphicsDevice = _renderManager->GetGraphicsDevice();
        GraphicsSwapchain* swapchain = _renderManager->GetSwapchain();

        // Get color target and swap extent
        RenderTarget const& colorTarget = _renderManager->GetColorTarget();
        Extent2D swapExtent = graphicsDevice->GetSwapchainExtent(swapchain);

        // Get command buffer
        GraphicsCommandBuffer* commandBuffer = graphicsDevice->BeginCommandBuffer();

        // Prepare color target for read
        {
            GraphicsTextureBarrier colorTargetShaderReadBarrier{};
            colorTargetShaderReadBarrier.texture = colorTarget.target;
            colorTargetShaderReadBarrier.srcLayout = GraphicsTextureLayout_RenderTarget;
            colorTargetShaderReadBarrier.srcResourceState = GraphicsResourceState_RenderTarget_Write;
            colorTargetShaderReadBarrier.dstLayout = GraphicsTextureLayout_ShaderReadOnly;
            colorTargetShaderReadBarrier.dstResourceState = GraphicsResourceState_PixelShader_Read;
            colorTargetShaderReadBarrier.subresource.aspectMask = GraphicsTextureAspect_Color;
            colorTargetShaderReadBarrier.subresource.baseMipLevel = 0;
            colorTargetShaderReadBarrier.subresource.levelCount = 1;
            colorTargetShaderReadBarrier.subresource.baseArrayLayer = 0;
            colorTargetShaderReadBarrier.subresource.layerCount = 1;

            GraphicsTextureBarrier shaderReadBarriers[] = { colorTargetShaderReadBarrier };
            commandBuffer->PipelineBarrier(nullptr, 0, nullptr, 0, shaderReadBarriers, std::size(shaderReadBarriers));
        }

        // Swap present pass
        {
            // Begin render pass
            GraphicsTextureView* swapTextureView = graphicsDevice->GetSwapchainBackbufferView(swapchain);
            GraphicsRenderAttachmentInfo colorAttachmentInfo{};
            colorAttachmentInfo.renderTarget = swapTextureView;
            colorAttachmentInfo.loadOp = GraphicsLoadOp_DontCare; // Render to fullscreen quad doesn't care about previous contents
            colorAttachmentInfo.storeOp = GraphicsStoreOp_Store;
            colorAttachmentInfo.clearValue.color = GraphicsClearColorValue{{ 0.0F, 0.0F, 0.0F, 0.0F }};

            Rect2D renderArea{};
            renderArea.offset = Offset2D{ 0, 0 };
            renderArea.extent = swapExtent;

            commandBuffer->BeginRendering(renderArea, &colorAttachmentInfo, 1, nullptr);

            // Set viewport and scissor
            GraphicsViewport viewport{};
            viewport.x = 0.0F;
            viewport.y = 0.0F;
            viewport.width = static_cast<float>(swapExtent.width);
            viewport.height = static_cast<float>(swapExtent.height);
            viewport.minDepth = 0.0F;
            viewport.maxDepth = 1.0F;
            
            Rect2D scissorRect{};
            scissorRect.offset = Offset2D{ 0, 0 };
            scissorRect.extent = swapExtent;

            commandBuffer->SetViewport(viewport);
            commandBuffer->SetScissorRect(scissorRect);

            // Bind pipeline state and draw
            commandBuffer->BindRenderPipeline(_presentRenderPipeline);
            commandBuffer->BindRenderDescriptorSets(_presentPipelineLayout, 0, &_presentDescriptorSet, 1);
            commandBuffer->Draw(3, 1, 0, 0);

            // End render pass
            commandBuffer->EndRendering();
        }

        // Submit commands
        graphicsDevice->SubmitFrameCommands(frameContext, commandBuffer);
    }

    void PresentRenderer::PostRender()
    {
        GraphicsDevice* graphicsDevice = _renderManager->GetGraphicsDevice();
        graphicsDevice->DestroyDescriptorSet(_presentDescriptorSet);
    }
} // namespace Bonsai
