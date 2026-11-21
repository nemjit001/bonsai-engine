#include "GraphicsCommandBufferPool.hpp"

namespace Bonsai
{
    GraphicsCommandBuffer* GraphicsCommandBufferPool::BeginNewCommandBuffer()
    {
        // Get next command buffer
        GraphicsCommandBufferImpl* commandBuffer = &_commandBuffers[_nextCommandBuffer];

        // Check if command buffer is ready for recording (i.e. in reset state)
        if (!commandBuffer->IsReady()) {
            return nullptr;
        }

        // Bump command buffer pointer and return command buffer
        _nextCommandBuffer = (_nextCommandBuffer + 1) % _commandBuffers.size();
        return commandBuffer;
    }
} // namespace Bonsai
