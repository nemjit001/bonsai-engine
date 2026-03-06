#include "RenderObject.hpp"

BRenderObject::BRenderObject()
    :
    BSceneObject()
{
    //
}

BRenderObject::BRenderObject(std::string const& ObjectName)
    :
    BSceneObject(ObjectName)
{
    //
}

void BRenderObject::SetTransform(BTransform const& NewTransform)
{
    Transform = NewTransform;
}
