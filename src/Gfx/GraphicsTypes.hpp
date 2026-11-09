#pragma once
#ifndef BONSAI_GRAPHICS_TYPES_HPP
#define BONSAI_GRAPHICS_TYPES_HPP

#include <cstdint>
#include <SDL3/SDL.h>

namespace Bonsai
{
    class GraphicsSwapchain;
    class FrameContext;

    /// @brief Swapchain description.
    struct GraphicsSwapchainDesc
    {
        SDL_Window* window;
        uint32_t width;
        uint32_t height;
        uint32_t syncInterval;
        bool allowTearing;
    };
} // namespace Bonsai

#endif //BONSAI_GRAPHICS_TYPES_HPP
