#include "RenderWorldSyncSystem.hpp"

#include "Renderer/RenderManager.hpp"

namespace Bonsai
{
    RenderWorldSyncSystem::RenderWorldSyncSystem(RenderManager* renderManager)
        :
        _renderManager(renderManager)
    {
        //
    }

    void RenderWorldSyncSystem::Run(entt::registry& registry)
    {
        //
    }
} // namespace Bonsai
