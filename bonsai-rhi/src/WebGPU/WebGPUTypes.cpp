#include "WebGPUTypes.hpp"
#if BONSAI_RHI_WEBGPU_SUPPORT

namespace RHI
{
    WebGPUBuffer::~WebGPUBuffer()
    {
        if (_buffer) {
            wgpuBufferRelease(_buffer);
        }
    }

    WebGPUTexture::~WebGPUTexture()
    {
        if (_view) {
            wgpuTextureViewRelease(_view);
        }

        if (_texture) {
            wgpuTextureRelease(_texture);
        }
    }

    WebGPUSampler::~WebGPUSampler()
    {
        if (_sampler) {
            wgpuSamplerRelease(_sampler);
        }
    }

    WebGPUDescriptorSetLayout::~WebGPUDescriptorSetLayout()
    {
        if (_bindGroupLayout) {
            wgpuBindGroupLayoutRelease(_bindGroupLayout);
        }
    }

    WebGPUDescriptorSet::~WebGPUDescriptorSet()
    {
        if (_bindGroup) {
            wgpuBindGroupRelease(_bindGroup);
        }
    }

    WebGPUPipelineLayout::~WebGPUPipelineLayout()
    {
        if (_pipelineLayout) {
            wgpuPipelineLayoutRelease(_pipelineLayout);
        }
    }

    WebGPUPipeline::~WebGPUPipeline()
    {
        if (_type == PipelineType_RenderPipeline && _pso.renderPipeline) {
            wgpuRenderPipelineRelease(_pso.renderPipeline);
        }

        if (_type == PipelineType_ComputePipeline && _pso.computePipeline) {
            wgpuComputePipelineRelease(_pso.computePipeline);
        }
    }
} // namespace RHI

#endif //BONSAI_RHI_WEBGPU_SUPPORT
