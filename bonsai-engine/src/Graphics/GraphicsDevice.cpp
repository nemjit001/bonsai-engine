#include "GraphicsDevice.hpp"

#include <BonsaiBase.hpp>
#include <sdl3webgpu.h>

static void UncapturedErrorCallback(
    [[maybe_unused]] WGPUDevice const* device,
    [[maybe_unused]] WGPUErrorType error,
    WGPUStringView message,
    [[maybe_unused]] void* userData1,
    [[maybe_unused]] void* userData2
)
{
    PrintDebugString("GraphicsDevice: WGPU Error: %*.s", message.length, message.data);
}

static void RequestAdapterCallback(
    WGPURequestAdapterStatus status,
    WGPUAdapter adapter,
    WGPUStringView message,
    void* userData1,
    [[maybe_unused]] void* userData2
)
{
    WGPUAdapter& outAdapter = *static_cast<WGPUAdapter*>(userData1);
    if (status == WGPURequestAdapterStatus_Success) {
        outAdapter = adapter;
    }
    else {
        PrintDebugString("GraphicsDevice: Failed to request WGPU adapter: %*.s", message.length, message.data);
    }
}

static void RequestDeviceCallback(
    WGPURequestDeviceStatus status,
    WGPUDevice device,
    WGPUStringView message,
    void* userData1,
    [[maybe_unused]] void* userData2
)
{
    WGPUDevice& outDevice = *static_cast<WGPUDevice*>(userData1);
    if (status == WGPURequestDeviceStatus_Success) {
        outDevice = device;
    }
    else {
        PrintDebugString("GraphicsDevice: Failed to request WGPU device: %*.s", message.length, message.data);
    }
}

GraphicsDevice::GraphicsDevice(WGPUInstance instance, WGPUAdapter adapter, WGPUDevice device)
    :
    _instance(instance),
    _adapter(adapter),
    _device(device),
    _queue(wgpuDeviceGetQueue(device))
{
    //
}

GraphicsDevice::~GraphicsDevice()
{
    wgpuQueueRelease(_queue);
    wgpuDeviceRelease(_device);
    wgpuAdapterRelease(_adapter);
    wgpuInstanceRelease(_instance);
}

GraphicsDevice* GraphicsDevice::Create()
{
    WGPUInstanceDescriptor instanceDesc{};
    instanceDesc.nextInChain = nullptr;
    instanceDesc.features.nextInChain = nullptr;
    instanceDesc.features.timedWaitAnyEnable = false;
    instanceDesc.features.timedWaitAnyMaxCount = 0;

    WGPUInstance instance = wgpuCreateInstance(&instanceDesc);
    if (!instance) {
        return nullptr;
    }

    WGPURequestAdapterOptions adapterOptions{};
    adapterOptions.nextInChain = nullptr;
    adapterOptions.featureLevel = WGPUFeatureLevel_Core;
    adapterOptions.powerPreference = WGPUPowerPreference_HighPerformance;
    adapterOptions.forceFallbackAdapter = false;
    adapterOptions.backendType = WGPUBackendType_Undefined;
    adapterOptions.compatibleSurface = nullptr;

    WGPUAdapter adapter = nullptr;
    WGPURequestAdapterCallbackInfo adapterCallbackInfo{};
    adapterCallbackInfo.nextInChain = nullptr;
    adapterCallbackInfo.mode = WGPUCallbackMode_AllowProcessEvents;
    adapterCallbackInfo.callback = RequestAdapterCallback;
    adapterCallbackInfo.userdata1 = &adapter;
    adapterCallbackInfo.userdata2 = nullptr;

    wgpuInstanceRequestAdapter(instance, &adapterOptions, adapterCallbackInfo);
    wgpuInstanceProcessEvents(instance);
    if (!adapter) {
        return nullptr;
    }

    WGPUDeviceDescriptor deviceDesc{};
    deviceDesc.nextInChain = nullptr;
    deviceDesc.label = WGPUStringView{ "Device", WGPU_STRLEN };
    deviceDesc.requiredFeatureCount = 0;
    deviceDesc.requiredFeatures = nullptr;
    deviceDesc.requiredLimits = nullptr;
    deviceDesc.defaultQueue.label = WGPUStringView{ "Default Queue", WGPU_STRLEN };
    deviceDesc.deviceLostCallbackInfo = {};
    deviceDesc.uncapturedErrorCallbackInfo.nextInChain = nullptr;
    deviceDesc.uncapturedErrorCallbackInfo.callback = UncapturedErrorCallback;
    deviceDesc.uncapturedErrorCallbackInfo.userdata1 = nullptr;
    deviceDesc.uncapturedErrorCallbackInfo.userdata2 = nullptr;

    WGPUDevice device = nullptr;
    WGPURequestDeviceCallbackInfo deviceCallbackInfo{};
    deviceCallbackInfo.nextInChain = nullptr;
    deviceCallbackInfo.mode = WGPUCallbackMode_AllowProcessEvents;
    deviceCallbackInfo.callback = RequestDeviceCallback;
    deviceCallbackInfo.userdata1 = &device;
    deviceCallbackInfo.userdata2 = nullptr;

    wgpuAdapterRequestDevice(adapter, &deviceDesc, deviceCallbackInfo);
    wgpuInstanceProcessEvents(instance);
    if (!device) {
        return nullptr;
    }

    return new GraphicsDevice(instance, adapter, device);
}
