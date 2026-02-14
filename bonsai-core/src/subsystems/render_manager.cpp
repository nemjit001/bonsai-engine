#include "bonsai/subsystems/render_manager.hpp"

#include "bonsai/subsystems/window_manager.hpp"

namespace bonsai::subsystems
{
    RenderManager::RenderManager(WindowManager* window_manager)
        :
        m_window_manager(window_manager)
    {
        //
    }

    bool RenderManager::init()
    {
        if (!IEngineSubsystem::init()) {
            return false;
        }

        // TODO(nemjit001): Internal renderer implementation should be initialized through render manager, renderer initializes the app window it needs
        WindowConfig window_config{ /*resizable*/ true, /*allow_high_dpi*/ true, WindowClientAPI::Vulkan };
        WindowID window = m_window_manager->create_window("Bonsai Window", 1280, 720, window_config);

        return true;
    }

    void RenderManager::shutdown()
    {
        //
    }

    void RenderManager::run()
    {
        //
    }
} // namespace bonsai::subsystems
