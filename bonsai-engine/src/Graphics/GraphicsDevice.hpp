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

private:
    WGPUInstance _instance = nullptr;
    WGPUAdapter _adapter = nullptr;
    WGPUDevice _device = nullptr;
    WGPUQueue _queue = nullptr;
};

#endif //GRAPHICS_DEVICE_HPP
