#pragma once
#ifndef WEBGPU_TYPES_HPP
#define WEBGPU_TYPES_HPP
#if BONSAI_RHI_WEBGPU_SUPPORT

#include <webgpu/webgpu.h>
#include "BonsaiRHI.hpp"
#include "BonsaiRHITypes.hpp"

namespace RHI
{
    /// @brief WebGPU GPU buffer implementation.
    class WebGPUBuffer : public GPUBuffer
    {
    public:
        constexpr WebGPUBuffer(BackendAPI backendAPI, WGPUBuffer buffer)
            : GPUBuffer(backendAPI), _buffer(buffer) {}
        ~WebGPUBuffer() override;

        /// @brief Get the internal buffer handle.
        /// @return 
        WGPUBuffer GetBuffer() const { return _buffer; }

    private:
        WGPUBuffer _buffer = nullptr;
    };

    /// @brief WebGPU GPU texture implementation.
    class WebGPUTexture : public GPUTexture
    {
    public:
        constexpr WebGPUTexture(BackendAPI backendAPI, WGPUTexture texture, WGPUTextureView view)
            : GPUTexture(backendAPI), _texture(texture), _view(view) {}
        ~WebGPUTexture() override;

        /// @brief Get the internal texture handle.
        /// @return 
        WGPUTexture GetTexture() const { return _texture; }

        /// @brief Get the internal texture view handle.
        /// @return 
        WGPUTextureView GetTextureView() const { return _view; }

    private:
        WGPUTexture _texture = nullptr;
        WGPUTextureView _view = nullptr;
    };

    /// @brief WebGPU GPU sampler implementation.
    class WebGPUSampler : public GPUSampler
    {
    public:
        constexpr WebGPUSampler(BackendAPI backendAPI, WGPUSampler sampler)
            : GPUSampler(backendAPI), _sampler(sampler) {}
        ~WebGPUSampler() override;

        /// @brief Get the internal sampler handle.
        /// @return 
        WGPUSampler GetSampler() const { return _sampler; }

    private:
        WGPUSampler _sampler = nullptr;
    };

    /// @brief WebGPU GPU pipeline implementation.
    class WebGPUPipeline : public GPUPipeline
    {
    public:
        constexpr WebGPUPipeline(BackendAPI backendAPI, WGPURenderPipeline pipeline)
            : GPUPipeline(backendAPI), _type(PipelineType_RenderPipeline) { _pso.renderPipeline = pipeline; }
        constexpr WebGPUPipeline(BackendAPI backendAPI, WGPUComputePipeline pipeline)
            : GPUPipeline(backendAPI), _type(PipelineType_ComputePipeline) { _pso.computePipeline = pipeline; }
        ~WebGPUPipeline() override;

        /// @brief Try to get the internal render pipeline handle.
        /// @return The internal handle, or nullptr on failure.
        WGPURenderPipeline TryGetRenderPipeline() const { return _type == PipelineType_RenderPipeline ? _pso.renderPipeline : nullptr; }

        /// @brief Try to get the internal compute pipeline handle.
        /// @return The internal handle, or nullptr on failure.
        WGPUComputePipeline TryGetComputePipeline() const { return _type == PipelineType_ComputePipeline ? _pso.computePipeline : nullptr; }

    private:
        /// @brief Internal pipeline type tracking.
        enum PipelineType : uint32
        {
            PipelineType_RenderPipeline     = 0,
            PipelineType_ComputePipeline    = 1,
        };

        /// @brief Internal pipeline state union.
        union PipelineState
        {
            WGPURenderPipeline renderPipeline;
            WGPUComputePipeline computePipeline;
        };

        PipelineType _type = PipelineType_RenderPipeline;
        PipelineState _pso = {};
    };
} // namespace RHI

#endif //BONSAI_RHI_WEBGPU_SUPPORT
#endif //WEBGPU_TYPES_HPP
