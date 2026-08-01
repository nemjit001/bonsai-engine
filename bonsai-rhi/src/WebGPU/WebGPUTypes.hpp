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
        constexpr WebGPUBuffer(WGPUBuffer buffer)
            : GPUBuffer(BackendAPI_WebGPU), _buffer(buffer) {}
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
        constexpr WebGPUTexture(WGPUTexture texture, WGPUTextureView view)
            : GPUTexture(BackendAPI_WebGPU), _texture(texture), _view(view) {}
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
        constexpr WebGPUSampler(WGPUSampler sampler)
            : GPUSampler(BackendAPI_WebGPU), _sampler(sampler) {}
        ~WebGPUSampler() override;

        /// @brief Get the internal sampler handle.
        /// @return 
        WGPUSampler GetSampler() const { return _sampler; }

    private:
        WGPUSampler _sampler = nullptr;
    };

    /// @brief WebGPU GPU descriptor set layout implementation.
    class WebGPUDescriptorSetLayout : public GPUDescriptorSetLayout
    {
    public:
        constexpr WebGPUDescriptorSetLayout(WGPUBindGroupLayout bindGroupLayout)
            : GPUDescriptorSetLayout(BackendAPI_WebGPU), _bindGroupLayout(bindGroupLayout) {}
        ~WebGPUDescriptorSetLayout() override;

        /// @brief Get the internal bind group layout handle.
        /// @return 
        WGPUBindGroupLayout GetBindGroupLayout() const { return _bindGroupLayout; }

    private:
        WGPUBindGroupLayout _bindGroupLayout = nullptr;
    };

    /// @brief WebGPU GPU descriptor set implementation.
    class WebGPUDescriptorSet : public GPUDescriptorSet
    {
    public:
        constexpr WebGPUDescriptorSet(WGPUBindGroup bindGroup)
            : GPUDescriptorSet(BackendAPI_WebGPU), _bindGroup(bindGroup) {}
        ~WebGPUDescriptorSet() override;

        /// @brief Get the internal bind group handle.
        /// @return 
        WGPUBindGroup GetBindGroup() const { return _bindGroup; }

    private:
        WGPUBindGroup _bindGroup = nullptr;
    };

    /// @brief WebGPU GPU pipeline layout implementation.
    class WebGPUPipelineLayout : public GPUPipelineLayout
    {
    public:
        constexpr WebGPUPipelineLayout(WGPUPipelineLayout pipelineLayout)
            : GPUPipelineLayout(BackendAPI_WebGPU), _pipelineLayout(pipelineLayout) {}
        ~WebGPUPipelineLayout() override;

        /// @brief Get the internal pipeline layout handle.
        /// @return 
        WGPUPipelineLayout GetPipelineLayout() const { return _pipelineLayout; }

    private:
        WGPUPipelineLayout _pipelineLayout = nullptr;
    };

    /// @brief WebGPU GPU pipeline implementation.
    class WebGPUPipeline : public GPUPipeline
    {
    public:
        constexpr WebGPUPipeline(WGPURenderPipeline pipeline)
            : GPUPipeline(BackendAPI_WebGPU), _type(PipelineType_RenderPipeline) { _pso.renderPipeline = pipeline; }
        constexpr WebGPUPipeline(WGPUComputePipeline pipeline)
            : GPUPipeline(BackendAPI_WebGPU), _type(PipelineType_ComputePipeline) { _pso.computePipeline = pipeline; }
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
