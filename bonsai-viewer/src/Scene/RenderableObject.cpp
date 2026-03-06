#include "RenderableObject.hpp"

BRenderableObject::BRenderableObject()
    :
    BRenderableObject("Render Object")
{
    //
}

BRenderableObject::BRenderableObject(std::string const& ObjectName)
    :
    BSceneObject(ObjectName)
{
    //
}
