#pragma once
#ifndef BONSAI_RENDER_MANAGER_HPP
#define BONSAI_RENDER_MANAGER_HPP

namespace Bonsai
{
    /// @brief The RenderManager manages high level render resource management and frame execution.
    class RenderManager
    {
    public:
        /// @brief Initialize the render manager.
        /// @return A boolean indicating successful initialization.
        bool Initialize();

        /// @brief Shut down the render manager.
        void Shutdown();

        /// @brief Start a frame.
        /// @return A boolean indicating successful frame start.
        bool StartFrame();

        /// @brief Record the render commands for the current frame.
        void RenderFrame();
    };
} // namespace Bonsai

#endif //BONSAI_RENDER_MANAGER_HPP
