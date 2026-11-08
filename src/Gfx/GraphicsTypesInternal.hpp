#pragma once
#ifndef BONSAI_GRAPHICS_TYPES_INTERNAL_HPP
#define BONSAI_GRAPHICS_TYPES_INTERNAL_HPP

#include "Gfx/GraphicsTypes.hpp"

namespace Bonsai
{
    struct FrameContext
    {
        //
    };

    class GraphicsSwapchain
    {
    public:
        virtual ~GraphicsSwapchain() = default;
    };
} // namespace Bonsai

#endif //BONSAI_GRAPHICS_TYPES_INTERNAL_HPP
