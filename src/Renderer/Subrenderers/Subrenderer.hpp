#pragma once
#ifndef BONSAI_SUBRENDERER_HPP
#define BONSAI_SUBRENDERER_HPP

#include <Gfx/GraphicsTypes.hpp>
#include <Gfx/ShaderCompiler.hpp>

namespace Bonsai
{
    class RenderManager;
    class RenderWorld;
    
    /// @brief The SubRenderer interface can be implemented by specialized renderers.
    class Subrenderer
    {
    public:
        virtual ~Subrenderer() = default;

        virtual bool Initialize(RenderManager* renderManager, ShaderCompiler* shaderCompiler) = 0;

        virtual void Shutdown() = 0;

        virtual void PrepareRender(FrameContext* frameContext, RenderWorld const& renderWorld) = 0;

        virtual void Render(FrameContext* frameContext, Extent2D const& renderExtent, RenderWorld const& renderWorld) = 0;

        virtual void PostRender() = 0;
    };
} // namespace Bonsai

#endif //BONSAI_SUBRENDERER_HPP
