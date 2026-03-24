#pragma once
#ifndef RENDER_HANDLE_HPP
#define RENDER_HANDLE_HPP

#include <cstdint>

/// @brief Typed render handle for managed renderer resouces.
/// @tparam RenderType 
template<class RenderType>
struct TRenderHandle
{
    uint32_t ResourceId         : 24;
    uint32_t ResourceVersion    : 8;
};

#endif //RENDER_HANDLE_HPP
