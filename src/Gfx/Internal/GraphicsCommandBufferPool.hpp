#pragma once
#ifndef BONSAI_GRAPHICS_COMMAND_BUFFER_POOL_HPP
#define BONSAI_GRAPHICS_COMMAND_BUFFER_POOL_HPP

#include "Gfx/GraphicsTypes.hpp"
#include "Gfx/Internal/GraphicsCommandBufferInternal.hpp"

namespace Bonsai
{
    /// @brief The GraphicsCommandBufferPool uses a ring buffer to provide command buffers
    /// for command recording. Command buffers become available for recording again after calling GraphicsCommandBuffer::Reset().
    class GraphicsCommandBufferPool
    {
    public:
        /// @brief Begin a new command buffer.
        /// @return The next available command buffer from the pool.
        GraphicsCommandBuffer* BeginNewCommandBuffer();

    private:
        size_t _nextCommandBuffer = 0;
        std::array<GraphicsCommandBufferImpl, Graphics::MaxCommandBuffersPerFrame> _commandBuffers;
    };
} // namespace Bonsai

#endif //BONSAI_GRAPHICS_COMMAND_BUFFER_POOL_HPP
