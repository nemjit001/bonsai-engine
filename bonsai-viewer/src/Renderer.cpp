#include "Renderer.hpp"

void BRenderer::Init(IRenderBackend* pRenderBackend)
{
    RenderBackend = pRenderBackend;
}

void BRenderer::Shutdown()
{
    //
}

void BRenderer::Frame()
{
    RenderBackend->NewFrame();
    

    RenderBackend->Present();
}
