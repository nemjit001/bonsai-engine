#include "RenderManager.hpp"

#include <Core/Filesystem.hpp>
#include <Core/Logger.hpp>
#include <Gfx/GraphicsDevice.hpp>
#include <Gfx/ShaderCompiler.hpp>

namespace Bonsai
{
    static constexpr char const*    WindowTitle         = "Bonsai Engine";
    static constexpr uint32_t       DefaultWindowWidth  = 1280u;
    static constexpr uint32_t       DefaultWindowHeight = 720u;

    bool RenderManager::Initialize()
    {
        SPDLOG_TRACE("Creating application window");
        // Set the window flags
        SDL_WindowFlags const windowFlags = SDL_WINDOW_RESIZABLE
            | SDL_WINDOW_HIGH_PIXEL_DENSITY
            | SDL_WINDOW_VULKAN;

        // Create the window
        _window = SDL_CreateWindow(WindowTitle, DefaultWindowWidth, DefaultWindowHeight, windowFlags);
        if (_window == nullptr)
        {
            SPDLOG_ERROR("Failed to create application window");
            return false;
        }

        // Create a graphics device
        SPDLOG_TRACE("Creating graphics device");
        _graphicsDevice = GraphicsDevice::TryCreate();
        if (_graphicsDevice == nullptr)
        {
            SPDLOG_ERROR("Failed to create graphics device");
            SDL_DestroyWindow(_window);
            return false;
        }

        // Create a swapchain
        SPDLOG_TRACE("Creating graphics swapchain");
        GraphicsSwapchainDesc swapchainDesc{};
        swapchainDesc.window = _window;
        swapchainDesc.preferredformat = GraphicsFormat_RGBA8_UNorm_SRGB;
        swapchainDesc.fallbackFormat = GraphicsFormat_BGRA8_UNorm_SRGB;
        swapchainDesc.width = DefaultWindowWidth;
        swapchainDesc.height = DefaultWindowHeight;
        swapchainDesc.syncInterval = 0;
        swapchainDesc.allowTearing = false;

        _swapchain = _graphicsDevice->CreateSwapchain(swapchainDesc);
        if (_swapchain == nullptr)
        {
            SPDLOG_ERROR("Failed to create graphics swapchain");
            GraphicsDevice::Destroy(_graphicsDevice);
            SDL_DestroyWindow(_window);
            return false;
        }

        // Set up render targets
        if (!CreateRenderTargets(Extent2D{ DefaultWindowWidth, DefaultWindowHeight }))
        {
            SPDLOG_ERROR("Failed to create render targets");
            GraphicsDevice::Destroy(_graphicsDevice);
            SDL_DestroyWindow(_window);
            return false;
        }

        // Set up render pipelines
        ShaderCompiler* shaderCompiler = ShaderCompiler::TryCreate();
        shaderCompiler->SetCompileTarget(ShaderCompileTarget_Vulkan13); // TODO(nemjit001): Set shader compile target based on selected render backend
        if (!CreatePipelines(shaderCompiler))
        {
            SPDLOG_ERROR("Failed to create graphics pipelines");
            ShaderCompiler::Destroy(shaderCompiler);
            GraphicsDevice::Destroy(_graphicsDevice);
            SDL_DestroyWindow(_window);
            return false;
        }

        ShaderCompiler::Destroy(shaderCompiler);
        return true;
    }

    void RenderManager::Shutdown()
    {
        // Wait for device to be idle
        _graphicsDevice->WaitForIdle();
        DestroyPipelines();
        DestroyRenderTargets();
        DestroyRenderWorld(_renderWorld);

        // Clean up device and swapchain
        _graphicsDevice->DestroySwapchain(_swapchain);
        Bonsai::GraphicsDevice::Destroy(_graphicsDevice);
        SDL_DestroyWindow(_window);
    }

    bool RenderManager::StartFrame()
    {
        // Try to start a new frame
        _activeFrame = _graphicsDevice->StartFrame(&_swapchain, 1);
        if (!_activeFrame) {
            return false;
        }

        return true;
    }

    void RenderManager::RenderFrame()
    {
        // Get render world
        RenderWorld const& renderWorld = GetRenderWorld();

        // Create scene descriptor set
        GraphicsDescriptorSet* sceneDescriptorSet = nullptr;
        {
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

            sceneDescriptorSet = _graphicsDevice->CreateDescriptorSet(_activeFrame, sceneDescriptorSetDesc);
            if (sceneDescriptorSet == nullptr) {
                SPDLOG_WARN("Failed to create scene descriptor set");
            }
        }

        // Create present descriptor set
        GraphicsDescriptorSet* presentDescriptorSet = nullptr;
        {
            GraphicsDescriptorImageInfo screenTextureImageInfo{};
            screenTextureImageInfo.sampler = _fullscreenSampler;
            screenTextureImageInfo.textureView = _colorTargetView;
            screenTextureImageInfo.textureLayout = GraphicsTextureLayout_ShaderReadOnly;

            GraphicsDescriptorSetBinding presentDescriptorSetBindings[] = {
                GraphicsDescriptorSetBinding{ 0, 0, 1, GraphicsDescriptorType_CombinedImageSampler, nullptr, &screenTextureImageInfo },
            };

            GraphicsDescriptorSetDesc presentDescriptorSetDesc{};
            presentDescriptorSetDesc.descriptorSetLayout = _presentDescriptorSetLayout;
            presentDescriptorSetDesc.bindingCount = std::size(presentDescriptorSetBindings);
            presentDescriptorSetDesc.bindings = presentDescriptorSetBindings;

            presentDescriptorSet = _graphicsDevice->CreateDescriptorSet(_activeFrame, presentDescriptorSetDesc);
            if (presentDescriptorSet == nullptr) {
                SPDLOG_WARN("Failed to create present descriptor set");
            }
        }

        // Record frame commands
        GraphicsCommandBuffer* commandBuffer = _graphicsDevice->BeginCommandBuffer();

        // Prepare render targets for write
        {
            GraphicsTextureBarrier colorTargetWriteBarrier{};
            colorTargetWriteBarrier.texture = _colorTarget;
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
            depthStencilTargetWriteBarrier.texture = _depthStencilTarget;
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
            Extent2D const renderExtent = GetWindowResolution();

            // Begin render pass
            GraphicsRenderAttachmentInfo colorAttachmentInfo{};
            colorAttachmentInfo.renderTarget = _colorTargetView;
            colorAttachmentInfo.loadOp = GraphicsLoadOp_Clear;
            colorAttachmentInfo.storeOp = GraphicsStoreOp_Store;
            colorAttachmentInfo.clearValue.color = GraphicsClearColorValue{{ 0.25F, 0.25F, 0.25F, 0.25F }};

            GraphicsRenderAttachmentInfo depthStencilAttachmentInfo{};
            depthStencilAttachmentInfo.renderTarget = _depthStencilTargetView;
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
            commandBuffer->BindRenderDescriptorSets(_renderPipelineLayout, 0, &sceneDescriptorSet, 1);

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

        // Prepare color target for read
        {
            GraphicsTextureBarrier colorTargetShaderReadBarrier{};
            colorTargetShaderReadBarrier.texture = _colorTarget;
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
            Extent2D const swapExtent = _graphicsDevice->GetSwapchainExtent(_swapchain);

            // Begin render pass
            GraphicsTextureView* swapTextureView = _graphicsDevice->GetSwapchainBackbufferView(_swapchain);
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
            commandBuffer->BindRenderDescriptorSets(_presentPipelineLayout, 0, &presentDescriptorSet, 1);
            commandBuffer->Draw(3, 1, 0, 0);

            // End render pass
            commandBuffer->EndRendering();
        }

        // Submit the active frame
        _graphicsDevice->SubmitFrameCommands(_activeFrame, commandBuffer);
        _graphicsDevice->SubmitFrame(_activeFrame);

        // Perform cleanup
        _graphicsDevice->DestroyDescriptorSet(presentDescriptorSet);
        _graphicsDevice->DestroyDescriptorSet(sceneDescriptorSet);
    }

    void RenderManager::HandlePlatformEvent(SDL_Event const& event)
    {
        if (event.type == SDL_EVENT_WINDOW_RESIZED)
        {
            Extent2D const extent = GetWindowResolution();
            HandleWindowResize(extent);
        }
        else if (event.type == SDL_EVENT_WINDOW_RESTORED)
        {
            Extent2D const extent = GetWindowResolution();
            HandleWindowResize(extent);
        }
    }

    Extent2D RenderManager::GetWindowResolution() const
    {
        int32_t width = 0;
        int32_t height = 0;
        SDL_GetWindowSizeInPixels(_window, &width, &height);

        return Extent2D {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height),
        };
    }

    void RenderManager::HandleWindowResize(Extent2D const& windowSize)
    {
        if (windowSize.width == 0 || windowSize.height == 0) {
            return;
        }

        _graphicsDevice->WaitForIdle();
        _graphicsDevice->ResizeSwapchain(_swapchain, windowSize.width, windowSize.height);
        CreateRenderTargets(windowSize);
    }

    bool RenderManager::CreateRenderTargets(Extent2D const& extent)
    {
        SPDLOG_TRACE("Creating render targets");
        if (extent.width == 0 || extent.height == 0) {
            return false;
        }

        // Ensure old targets are destroyed
        DestroyRenderTargets();

        // Create render targets
        GraphicsTextureDesc colorTargetDesc{};
        colorTargetDesc.heapType = GraphicsHeapType_Default;
        colorTargetDesc.textureType = GraphicsTextureType_2D;
        colorTargetDesc.textureFormat = GraphicsFormat_RGBA16_SFloat;
        colorTargetDesc.width = extent.width;
        colorTargetDesc.height = extent.height;
        colorTargetDesc.depthOrLayers = 1;
        colorTargetDesc.mipLevels = 1;
        colorTargetDesc.sampleCount = 1;
        colorTargetDesc.tilingMode = GraphicsTextureTiling_Optimal;
        colorTargetDesc.usage = GraphicsTextureUsage_RenderTarget | GraphicsTextureUsage_SampledImage;

        GraphicsTextureDesc depthStencilTargetDesc{};
        depthStencilTargetDesc.heapType = GraphicsHeapType_Default;
        depthStencilTargetDesc.textureType = GraphicsTextureType_2D;
        depthStencilTargetDesc.textureFormat = GraphicsFormat_D24_UNorm_S8_UInt;
        depthStencilTargetDesc.width = extent.width;
        depthStencilTargetDesc.height = extent.height;
        depthStencilTargetDesc.depthOrLayers = 1;
        depthStencilTargetDesc.mipLevels = 1;
        depthStencilTargetDesc.sampleCount = 1;
        depthStencilTargetDesc.tilingMode = GraphicsTextureTiling_Optimal;
        depthStencilTargetDesc.usage = GraphicsTextureUsage_DepthStencilTarget;

        _colorTarget = _graphicsDevice->CreateTexture(colorTargetDesc);
        _depthStencilTarget = _graphicsDevice->CreateTexture(depthStencilTargetDesc);
        if (_colorTarget == nullptr || _depthStencilTarget == nullptr)
        {
            DestroyRenderTargets();
            return false;
        }

        // Create render target views
        GraphicsTextureViewDesc colorTargetViewDesc{};
        colorTargetViewDesc.texture = _colorTarget;
        colorTargetViewDesc.viewType = GraphicsTextureViewType_2D;
        colorTargetViewDesc.viewFormat = colorTargetDesc.textureFormat;
        colorTargetViewDesc.subresourceRange.aspectMask = GraphicsTextureAspect_Color;
        colorTargetViewDesc.subresourceRange.baseMipLevel = 0;
        colorTargetViewDesc.subresourceRange.levelCount = 1;
        colorTargetViewDesc.subresourceRange.baseArrayLayer = 0;
        colorTargetViewDesc.subresourceRange.layerCount = 1;

        GraphicsTextureViewDesc depthStencilTargetViewDesc{};
        depthStencilTargetViewDesc.texture = _depthStencilTarget;
        depthStencilTargetViewDesc.viewType = GraphicsTextureViewType_2D;
        depthStencilTargetViewDesc.viewFormat = depthStencilTargetDesc.textureFormat;
        depthStencilTargetViewDesc.subresourceRange.aspectMask = GraphicsTextureAspect_Depth | GraphicsTextureAspect_Stencil;
        depthStencilTargetViewDesc.subresourceRange.baseMipLevel = 0;
        depthStencilTargetViewDesc.subresourceRange.levelCount = 1;
        depthStencilTargetViewDesc.subresourceRange.baseArrayLayer = 0;
        depthStencilTargetViewDesc.subresourceRange.layerCount = 1;

        _colorTargetView = _graphicsDevice->CreateTextureView(colorTargetViewDesc);
        _depthStencilTargetView = _graphicsDevice->CreateTextureView(depthStencilTargetViewDesc);
        if (_colorTargetView == nullptr || _depthStencilTargetView == nullptr)
        {
            DestroyRenderTargets();
            return false;
        }

        return true;
    }

    bool RenderManager::CreatePipelines(ShaderCompiler* shaderCompiler)
    {
        SPDLOG_TRACE("Creating render pipelines");
        if (shaderCompiler == nullptr) {
            return false;
        }

        // Create scene data descriptor set layout
        GraphicsDescriptorSetLayoutBinding sceneDataDescriptorBindings[] = {
            GraphicsDescriptorSetLayoutBinding{ 0, GraphicsDescriptorType_StorageBuffer, 1, GraphicsShaderStage_Vertex, nullptr },
            GraphicsDescriptorSetLayoutBinding{ 1, GraphicsDescriptorType_StorageBuffer, 1, GraphicsShaderStage_Vertex, nullptr },
            GraphicsDescriptorSetLayoutBinding{ 2, GraphicsDescriptorType_StorageBuffer, 1, GraphicsShaderStage_Vertex, nullptr },
        };

        GraphicsDescriptorSetLayoutDesc sceneDataDescriptorSetLayoutDesc{};
        sceneDataDescriptorSetLayoutDesc.bindingCount = std::size(sceneDataDescriptorBindings);
        sceneDataDescriptorSetLayoutDesc.bindings = sceneDataDescriptorBindings;

        _sceneDataDescriptorSetLayout = _graphicsDevice->CreateDescriptorSetLayout(sceneDataDescriptorSetLayoutDesc);
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

        _renderPipelineLayout = _graphicsDevice->CreatePipelineLayout(renderPipelineLayoutDesc);
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

        _unlitRenderPipeline = _graphicsDevice->CreateRenderPipeline(unlitPipelineDesc);
        if (_unlitRenderPipeline == nullptr) {
            return false;
        }

        // Create fullscreen sampler
        GraphicsSamplerDesc fullscreenSamplerDesc{};
        fullscreenSamplerDesc.magFilter = GraphicsFilter_Linear;
        fullscreenSamplerDesc.minFilter = GraphicsFilter_Linear;
        fullscreenSamplerDesc.mipmapMode = GraphicsSamplerMipmapMode_Linear;

        _fullscreenSampler = _graphicsDevice->CreateSampler(fullscreenSamplerDesc);
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

        _presentDescriptorSetLayout = _graphicsDevice->CreateDescriptorSetLayout(presentDescriptorSetLayoutDesc);
        if (_presentDescriptorSetLayout == nullptr) {
            return false;
        }

        // Set up present pipeline layout
        GraphicsPipelineLayoutDesc presentPipelineLayoutDesc{};
        presentPipelineLayoutDesc.descriptorSetLayoutCount = 1;
        presentPipelineLayoutDesc.descriptorSetLayouts = &_presentDescriptorSetLayout;
        presentPipelineLayoutDesc.pushConstantRangeCount = 0;
        presentPipelineLayoutDesc.pushConstantRanges = nullptr;

        _presentPipelineLayout = _graphicsDevice->CreatePipelineLayout(presentPipelineLayoutDesc);
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
        presentPipelineDesc.renderTargetFormats[0] = _graphicsDevice->GetSwapchainFormat(_swapchain);
        presentPipelineDesc.depthStencilFormat = GraphicsFormat_Unknown;

        _presentRenderPipeline = _graphicsDevice->CreateRenderPipeline(presentPipelineDesc);
        if (_presentRenderPipeline == nullptr) {
            return false;
        }

        return true;
    }

    void RenderManager::DestroyRenderWorld(RenderWorld const& renderWorld)
    {
        // Clean up render world geometry cache
        for (auto const& [ _, renderMesh ] : renderWorld.geometryCache)
        {
            _graphicsDevice->DestroyBuffer(renderMesh.vertexBuffer);
            _graphicsDevice->DestroyBuffer(renderMesh.indexBuffer);
        }

        // Clean up render world buffers
        _graphicsDevice->DestroyBuffer(renderWorld.cameraBuffer);
        _graphicsDevice->DestroyBuffer(renderWorld.renderObjectBuffer);
        _graphicsDevice->DestroyBuffer(renderWorld.objectTransformBuffer);
    }

    void RenderManager::DestroyRenderTargets()
    {
        _graphicsDevice->DestroyTextureView(_depthStencilTargetView);
        _graphicsDevice->DestroyTextureView(_colorTargetView);
        _graphicsDevice->DestroyTexture(_depthStencilTarget);
        _graphicsDevice->DestroyTexture(_colorTarget);
    }

    void RenderManager::DestroyPipelines()
    {
        _graphicsDevice->DestroyRenderPipeline(_presentRenderPipeline);
        _graphicsDevice->DestroyPipelineLayout(_presentPipelineLayout);
        _graphicsDevice->DestroyDescriptorSetLayout(_presentDescriptorSetLayout);
        _graphicsDevice->DestroySampler(_fullscreenSampler);

        _graphicsDevice->DestroyRenderPipeline(_unlitRenderPipeline);
        _graphicsDevice->DestroyPipelineLayout(_renderPipelineLayout);
        _graphicsDevice->DestroyDescriptorSetLayout(_sceneDataDescriptorSetLayout);
    }
} // namespace Bonsai
