#pragma once
#ifndef RENDER_OBJECT_HPP
#define RENDER_OBJECT_HPP

#include "SceneObject.hpp"

/// @brief A RenderObject is an object that can be rendered by the renderer system.
class BRenderObject : public BSceneObject
{
public:
    BRenderObject();
    BRenderObject(std::string const& ObjectName);
};

#endif //RENDER_OBJECT_HPP
