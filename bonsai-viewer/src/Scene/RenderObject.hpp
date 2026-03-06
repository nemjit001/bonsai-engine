#pragma once
#ifndef RENDER_OBJECT_HPP
#define RENDER_OBJECT_HPP

#include "SceneObject.hpp"
#include "Transform.hpp"

/// @brief A RenderObject is an object with a world transform that can be rendered.
class BRenderObject : public BSceneObject
{
public:
    BRenderObject();
    BRenderObject(std::string const& ObjectName);

    /// @brief Set the transform for this RenderObject.
    /// @param NewTransform 
    void SetTransform(BTransform const& NewTransform);

    /// @brief Get the transform for this RenderObject.
    /// @return 
    BTransform& GetTransform() { return Transform; }

protected:
    BTransform Transform;
};

#endif //RENDER_OBJECT_HPP
