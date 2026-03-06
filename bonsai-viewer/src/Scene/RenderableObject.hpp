#pragma once
#ifndef RENDERABLE_OBJECT_HPP
#define RENDERABLE_OBJECT_HPP

#include "SceneObject.hpp"

/// @brief A RenderableObject is an object that can be rendered by the renderer system.
class BRenderableObject : public BSceneObject
{
public:
    BRenderableObject();
    BRenderableObject(std::string const& ObjectName);
};

#endif //RENDERABLE_OBJECT_HPP
