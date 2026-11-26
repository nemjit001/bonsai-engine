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

        // Create up render targets
        if (!CreateRenderTargets(Extent2D{ DefaultWindowWidth, DefaultWindowHeight }))
        {
            SPDLOG_ERROR("Failed to create render targets");
            GraphicsDevice::Destroy(_graphicsDevice);
            SDL_DestroyWindow(_window);
            return false;
        }

        // Initialize sub renderers
        ShaderCompiler* shaderCompiler = ShaderCompiler::TryCreate();
        shaderCompiler->SetCompileTarget(ShaderCompileTarget_Vulkan13); // TODO(nemjit001): Set shader compile target based on selected render backend
        if (!_geometryRenderer.Initialize(this, shaderCompiler)
            || !_presentRenderer.Initialize(this, shaderCompiler))
        {
            SPDLOG_ERROR("Failed to initialize sub renderers");
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

        // Shut down sub renderers
        _presentRenderer.Shutdown();
        _geometryRenderer.Shutdown();

        // Clean up shared render states
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
        // Get render extent and render world
        Extent2D const renderExtent = GetWindowResolution();
        RenderWorld const& renderWorld = GetRenderWorld();

        // Prepare for render
        _geometryRenderer.PrepareRender(_activeFrame, renderWorld);
        _presentRenderer.PrepareRender(_activeFrame, renderWorld);

        // Render
        _geometryRenderer.Render(_activeFrame, renderExtent, renderWorld);
        _presentRenderer.Render(_activeFrame, renderExtent, renderWorld);

        // Submit the active frame
        _graphicsDevice->SubmitFrame(_activeFrame);

        // Cleanup
        _geometryRenderer.PostRender();
        _presentRenderer.PostRender();
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

        // Create color target
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

        _colorTarget.target = _graphicsDevice->CreateTexture(colorTargetDesc);
        _colorTarget.targetView = _graphicsDevice->CreateDefaultTextureView(_colorTarget.target);
        if (_colorTarget.target == nullptr || _colorTarget.targetView == nullptr)
        {
            DestroyRenderTargets();
            return false;
        }

        // Create depth stencil target
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

        _depthStencilTarget.target = _graphicsDevice->CreateTexture(depthStencilTargetDesc);
        _depthStencilTarget.targetView = _graphicsDevice->CreateDefaultTextureView(_depthStencilTarget.target);
        if (_depthStencilTarget.target == nullptr || _depthStencilTarget.targetView == nullptr)
        {
            DestroyRenderTargets();
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
        _graphicsDevice->DestroyBuffer(renderWorld.subMeshBuffer);
        _graphicsDevice->DestroyBuffer(renderWorld.materialBuffer);
    }

    void RenderManager::DestroyRenderTargets()
    {
        _graphicsDevice->DestroyTextureView(_depthStencilTarget.targetView);
        _graphicsDevice->DestroyTexture(_depthStencilTarget.target);

        _graphicsDevice->DestroyTextureView(_colorTarget.targetView);
        _graphicsDevice->DestroyTexture(_colorTarget.target);
    }
} // namespace Bonsai
