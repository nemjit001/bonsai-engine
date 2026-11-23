#include "RenderManager.hpp"

#include <Core/Filesystem.hpp>
#include <Core/Logger.hpp>
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
        // Record frame commands
        GraphicsCommandBuffer* commandBuffer = _graphicsDevice->BeginCommandBuffer();
        GraphicsTextureView* swapTextureView = _graphicsDevice->GetSwapchainBackbufferView(_swapchain);

        GraphicsRenderAttachmentInfo colorAttachmentInfo{};
        colorAttachmentInfo.renderTarget = swapTextureView;
        colorAttachmentInfo.loadOp = GraphicsLoadOp_Clear;
        colorAttachmentInfo.storeOp = GraphicsStoreOp_Store;
        colorAttachmentInfo.clearValue = GraphicsClearValue{{{ 0.5F, 0.5, 0.5F, 0.5F }}};

        Rect2D renderArea{};
        renderArea.offset = Offset2D{ 0, 0 };
        renderArea.extent = _graphicsDevice->GetSwapchainExtent(_swapchain);

        commandBuffer->BeginRendering(renderArea, &colorAttachmentInfo, 1, nullptr);
        commandBuffer->EndRendering();
        _graphicsDevice->SubmitFrameCommands(_activeFrame, commandBuffer);

        // Submit the active frame
        _graphicsDevice->DestroyTextureView(swapTextureView);
        _graphicsDevice->SubmitFrame(_activeFrame);
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
        colorTargetDesc.usage = GraphicsTextureUsage_RenderTarget;

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
        if (shaderCompiler == nullptr) {
            return false;
        }

        // Create descriptor set layout
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

        // Set up pipeline layout
        GraphicsPushConstantRange pushConstantRanges[] = {
            GraphicsPushConstantRange{ GraphicsShaderStage_Vertex, 0, sizeof(Shader::DrawParams) },
        };

        GraphicsPipelineLayoutDesc pipelineLayoutDesc{};
        pipelineLayoutDesc.descriptorSetLayoutCount = 1;
        pipelineLayoutDesc.descriptorSetLayouts = &_sceneDataDescriptorSetLayout;
        pipelineLayoutDesc.pushConstantRangeCount = std::size(pushConstantRanges);
        pipelineLayoutDesc.pushConstantRanges = pushConstantRanges;

        _pipelineLayout = _graphicsDevice->CreatePipelineLayout(pipelineLayoutDesc);
        if (_pipelineLayout == nullptr) {
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
        unlitPipelineDesc.pipelineLayout = _pipelineLayout;
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
        unlitPipelineDesc.rasterState.frontCounterClockwise = true;
        unlitPipelineDesc.multisampleState.sampleCount = 1;
        unlitPipelineDesc.depthStencilState.depthTest = true;
        unlitPipelineDesc.depthStencilState.depthWrite = true;
        unlitPipelineDesc.depthStencilState.depthCompareOp = GraphicsCompareOp_Less;
        unlitPipelineDesc.blendState.logicOp = GraphicsLogicOp_Undefined;
        unlitPipelineDesc.blendState.renderTargets[0].blendEnable = false;
        unlitPipelineDesc.blendState.renderTargets[0].colorWriteMask = GraphicsColorComponent_All;
        unlitPipelineDesc.renderTargetCount = 1;
        unlitPipelineDesc.renderTargetFormats[0] = GraphicsFormat_RGBA32_SFloat;
        unlitPipelineDesc.depthStencilFormat = GraphicsFormat_D24_UNorm_S8_UInt;

        _unlitRenderPipeline = _graphicsDevice->CreateRenderPipeline(unlitPipelineDesc);
        if (_unlitRenderPipeline == nullptr) {
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
        _graphicsDevice->DestroyRenderPipeline(_unlitRenderPipeline);
        _graphicsDevice->DestroyPipelineLayout(_pipelineLayout);
        _graphicsDevice->DestroyDescriptorSetLayout(_sceneDataDescriptorSetLayout);
    }
} // namespace Bonsai
