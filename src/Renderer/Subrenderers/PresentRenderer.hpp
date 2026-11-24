#pragma once
#ifndef BONSAI_PRESENT_RENDERER_HPP
#define BONSAI_PRESENT_RENDERER_HPP

#include "Renderer/Subrenderers/Subrenderer.hpp"

namespace Bonsai
{
    class PresentRenderer : public Subrenderer
    {
    public:
        bool Initialize(RenderManager* renderManager, ShaderCompiler* shaderCompiler) override final;
        void Shutdown() override final;
        void PrepareRender(FrameContext* frameContext, RenderWorld const& renderWorld) override final;
        void Render(FrameContext* frameContext, Extent2D const& renderExtent, RenderWorld const& renderWorld) override final;
        void PostRender() override final;

    private:
        RenderManager* _renderManager = nullptr;
        
        //-- Render data --//
        GraphicsSampler* _fullscreenSampler = nullptr;
        GraphicsDescriptorSetLayout* _presentDescriptorSetLayout = nullptr;
        GraphicsPipelineLayout* _presentPipelineLayout = nullptr;
        GraphicsRenderPipeline* _presentRenderPipeline = nullptr;

        //-- Transient data --//
        GraphicsDescriptorSet* _presentDescriptorSet = nullptr;
    };
} // namespace Bonsai

#endif //BONSAI_PRESENT_RENDERER_HPP
