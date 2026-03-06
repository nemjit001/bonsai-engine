#include "RenderObject.hpp"

BRenderObject::BRenderObject()
    :
    BRenderObject("Render Object")
{
    //
}

BRenderObject::BRenderObject(std::string const& ObjectName)
    :
    BSceneObject(ObjectName)
{
    //
}
