#pragma once
#ifndef RENDERABLE_OBJECT_HPP
#define RENDERABLE_OBJECT_HPP

#include <memory>
#include "SceneObject.hpp"
#include "Assets/StaticMesh.hpp"

/// @brief A RenderableObject is an object that can be rendered by the renderer system.
class BRenderableObject : public BSceneObject
{
public:
    BRenderableObject();
    BRenderableObject(std::string const& ObjectName);

    virtual SceneObjectType GetSceneObjectType() const override { return SOType_Renderable; }

public:
    std::unique_ptr<BStaticMesh> Mesh;
};

#endif //RENDERABLE_OBJECT_HPP
