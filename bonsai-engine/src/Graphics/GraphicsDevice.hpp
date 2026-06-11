#pragma once
#ifndef GRAPHICS_DEVICE_HPP
#define GRAPHICS_DEVICE_HPP

#include <webgpu/webgpu.h>
#include <sdl3webgpu.h>

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
