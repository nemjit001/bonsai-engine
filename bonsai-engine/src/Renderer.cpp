#include "Renderer.hpp"

Renderer::Renderer() = default;
Renderer::~Renderer() = default;

bool Renderer::Init()
{
    // Initialize graphics device
    _graphicsDevice = GraphicsDevice::Create();
    if (!_graphicsDevice) {
        return false;
    }

    // Initialize window
    _mainWindow = SDL_CreateWindow("WebGPU window", 1280, 720, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (!_mainWindow) {
        return false;
    }

    // Initialize surface
    _mainWindowSurface = _graphicsDevice->CreateSurface(_mainWindow);
    if (!_mainWindowSurface) {
        return false;
    }

    if (!ConfigureSurface(_mainWindow, _mainWindowSurface, WGPUPresentMode_Fifo)) {
        return false;
    }

    return true;
}

void Renderer::Shutdown()
{
    wgpuSurfaceRelease(_mainWindowSurface);
    SDL_DestroyWindow(_mainWindow);
    delete _graphicsDevice;
}

void Renderer::Frame()
{
    BONSAI_ASSERT(_graphicsDevice != nullptr);
    WGPUTexture swapTexture = _graphicsDevice->AcquireNextImage(_mainWindowSurface);
    if (!swapTexture)
    {
        ConfigureSurface(_mainWindow, _mainWindowSurface, WGPUPresentMode_Fifo);
        return;
    }

    WGPUTextureView swapTextureView = wgpuTextureCreateView(swapTexture, nullptr);
    if (!swapTextureView) {
        return;
    }

    WGPUCommandEncoderDescriptor frameCommandEncoderDesc{};
    frameCommandEncoderDesc.nextInChain = nullptr;
    frameCommandEncoderDesc.label = WGPUStringView{ "Frame Command Encoder", WGPU_STRLEN };
    WGPUCommandEncoder frameCommandEncoder = wgpuDeviceCreateCommandEncoder(_graphicsDevice->GetDevice(), &frameCommandEncoderDesc);

    WGPURenderPassColorAttachment swapAttachment{};
    swapAttachment.nextInChain = nullptr;
    swapAttachment.view = swapTextureView;
    swapAttachment.resolveTarget = nullptr;
    swapAttachment.loadOp = WGPULoadOp_Clear;
    swapAttachment.storeOp = WGPUStoreOp_Store;
    swapAttachment.clearValue = WGPUColor{ 0.5F, 0.5F, 0.5F, 1.0F };
    swapAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

    WGPURenderPassDescriptor renderPassDesc{};
    renderPassDesc.nextInChain = nullptr;
    renderPassDesc.label = WGPUStringView{ "Render Pass", WGPU_STRLEN };
    renderPassDesc.colorAttachmentCount = 1;
    renderPassDesc.colorAttachments = &swapAttachment;
    renderPassDesc.depthStencilAttachment = nullptr;
    renderPassDesc.occlusionQuerySet = nullptr;
    renderPassDesc.timestampWrites = nullptr;
    WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(frameCommandEncoder, &renderPassDesc);

    wgpuRenderPassEncoderEnd(renderPassEncoder);
    wgpuRenderPassEncoderRelease(renderPassEncoder);

    WGPUCommandBufferDescriptor frameCommandBufferDesc{};
    frameCommandBufferDesc.nextInChain = nullptr;
    frameCommandBufferDesc.label = WGPUStringView{ "Frame Command Buffer", WGPU_STRLEN };

    WGPUCommandBuffer frameCommandBuffer = wgpuCommandEncoderFinish(frameCommandEncoder, &frameCommandBufferDesc);
    wgpuCommandEncoderRelease(frameCommandEncoder);

    wgpuQueueSubmit(_graphicsDevice->GetQueue(), 1, &frameCommandBuffer);
    wgpuCommandBufferRelease(frameCommandBuffer);

    _graphicsDevice->Present(_mainWindowSurface);
    wgpuTextureViewRelease(swapTextureView);
    wgpuTextureRelease(swapTexture);
}

bool Renderer::ConfigureSurface(SDL_Window* window, WGPUSurface surface, WGPUPresentMode presentMode) const
{
    // Get window size
    int32 width = 0, height = 0;
    SDL_GetWindowSizeInPixels(window, &width, &height);

    // Get surface capabilities
    WGPUSurfaceCapabilities surfaceCapabilities{};
    wgpuSurfaceGetCapabilities(surface, _graphicsDevice->GetAdapter(), &surfaceCapabilities);

    // Verify surface support
    bool presentModeSupported = false;
    for (usize i = 0; i < surfaceCapabilities.presentModeCount; i++)
    {
        if (surfaceCapabilities.presentModes[i] == presentMode)
        {
            presentModeSupported = true;
            break;
        }
    }

    if (!presentModeSupported) {
        return false;
    }

    // Configure surface
    WGPUSurfaceConfiguration surfaceConfig{};
    surfaceConfig.nextInChain = nullptr;
    surfaceConfig.device = _graphicsDevice->GetDevice();
    surfaceConfig.usage = WGPUTextureUsage_RenderAttachment;
    surfaceConfig.format = surfaceCapabilities.formats[0]; // Pick first available format
    surfaceConfig.width = width;
    surfaceConfig.height = height;
    surfaceConfig.presentMode = presentMode;
    surfaceConfig.alphaMode = WGPUCompositeAlphaMode_Opaque;
    surfaceConfig.viewFormatCount = 0;
    surfaceConfig.viewFormats = nullptr;

    wgpuSurfaceConfigure(surface, &surfaceConfig);
    wgpuSurfaceCapabilitiesFreeMembers(surfaceCapabilities);
    return true;
}
