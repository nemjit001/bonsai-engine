#pragma once
#ifndef GRAPHICS_DEVICE_HPP
#define GRAPHICS_DEVICE_HPP

#include <SDL3/SDL.h>
#include <webgpu/webgpu.h>

/// @brief A GraphicsDevice is a thin wrapper around WebGPU instance and device structures.
class GraphicsDevice
{
private:
    GraphicsDevice(WGPUInstance instance, WGPUAdapter adapter, WGPUDevice device);

public:
    ~GraphicsDevice();

    GraphicsDevice(GraphicsDevice const&) = delete;
    GraphicsDevice& operator=(GraphicsDevice const&) = delete;

    /// @brief Create a new GraphicsDevice.
    /// @return A new graphics device, or nullptr on failure.
    static GraphicsDevice* Create();

    /// @brief Create a surface from an SDL window.
    /// @param window Window to use for surface creation.
    /// @return The newly created surface, or nullptr on failure.
    WGPUSurface CreateSurface(SDL_Window* window);

    /// @brief Acquire a new image for a surface.
    /// @param surface Surface to acquire image from.
    /// @return The next surface texture, or nullptr on failure.
    WGPUTexture AcquireNextImage(WGPUSurface surface);

    /// @brief Present to a surface.
    /// @param surface Surface to present to.
    void Present(WGPUSurface surface);

    /// @brief Get the device limits.
    /// @return 
    WGPULimits GetDeviceLimits() const;

    /// @brief Retrieve the underlying WGPU device.
    /// @return 
    WGPUDevice GetDevice() const { return _device; }

    /// @brief Retrieve the underlying WGPU queue.
    /// @return 
    WGPUQueue GetQueue() const { return _queue; }

private:
    WGPUInstance _instance = nullptr;
    WGPUAdapter _adapter = nullptr;
    WGPUDevice _device = nullptr;
    WGPUQueue _queue = nullptr;
};

#endif //GRAPHICS_DEVICE_HPP
