#include "WebGPUTypes.hpp"
#if BONSAI_RHI_WEBGPU_SUPPORT

namespace RHI
{
    WebGPUBuffer::~WebGPUBuffer()
    {
        wgpuBufferRelease(_buffer);
    }

    WebGPUTexture::~WebGPUTexture()
    {
        wgpuTextureViewRelease(_view);
        wgpuTextureRelease(_texture);
    }

    WebGPUSampler::~WebGPUSampler()
    {
        wgpuSamplerRelease(_sampler);
    }

    WebGPUPipeline::~WebGPUPipeline()
    {
        if (_type == PipelineType_RenderPipeline) {
            wgpuRenderPipelineRelease(_pso.renderPipeline);
        }
        else if (_type == PipelineType_ComputePipeline) {
            wgpuComputePipelineRelease(_pso.computePipeline);
        }
    }
} // namespace RHI

#endif //BONSAI_RHI_WEBGPU_SUPPORT
