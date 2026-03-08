#pragma once
#ifndef PIPELINE_STATE_HPP
#define PIPELINE_STATE_HPP

#include <volk.h>

/// @brief Vulkan pipeline state implementation.
class BPipelineState
{
public:
    VkPipelineBindPoint BindPoint;
    VkPipelineLayout Layout;
    VkPipeline Pipeline;
};

#endif //PIPELINE_STATE_HPP
