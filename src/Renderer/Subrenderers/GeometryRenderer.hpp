#pragma once
#ifndef BONSAI_GEOMETRY_RENDERER_HPP
#define BONSAI_GEOMETRY_RENDERER_HPP

#include "Renderer/Subrenderers/Subrenderer.hpp"

namespace Bonsai
{
    class GeometryRenderer : public Subrenderer
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
        GraphicsDescriptorSetLayout* _sceneDataDescriptorSetLayout = nullptr;
        GraphicsPipelineLayout* _renderPipelineLayout = nullptr;
        GraphicsRenderPipeline* _unlitRenderPipeline = nullptr;

        //-- Transient data --//
        GraphicsDescriptorSet* _sceneDataDescriptorSet = nullptr;
    };
} // namespace Bonsai

#endif //BONSAI_GEOMETRY_RENDERER_HPP
