#pragma once
#ifndef RENDER_OBJECT_HPP
#define RENDER_OBJECT_HPP

#include "SceneObject.hpp"

/// @brief A RenderObject is an object with a world representation that can be rendered by the renderer system.
class BRenderObject : public BSceneObject
{
public:
    BRenderObject();
    BRenderObject(std::string const& ObjectName);
};

#endif //RENDER_OBJECT_HPP
