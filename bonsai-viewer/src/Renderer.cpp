#include "Renderer.hpp"

#include "RenderBackend.hpp"

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
    //
}
