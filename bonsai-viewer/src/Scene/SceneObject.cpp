#include "SceneObject.hpp"

#include <algorithm>

BSceneObject::BSceneObject()
    :
    BSceneObject("Scene Object")
{
    //
}

BSceneObject::BSceneObject(std::string const& ObjectName)
    :
    Name(ObjectName),
    Transform(),
    Parent(nullptr),
    Children()
{
    //
}

void BSceneObject::SetTransform(BTransform const& NewTransform)
{
    Transform = NewTransform;
}

glm::mat4 BSceneObject::GetWorldMatrix() const
{
    glm::mat4 const LocalTransform = GetTransform().GetMatrix();
    if (BSceneObject* NodeParent = GetParent()) {
        return LocalTransform * NodeParent->GetWorldMatrix();
    }

    return LocalTransform;
}

void BSceneObject::SetParent(BSceneObject* NewParent)
{
    Parent = NewParent;
}

void BSceneObject::AddChild(std::unique_ptr<BSceneObject>&& Child)
{
    Child->SetParent(this);
    Children.emplace_back(std::move(Child));
}

void BSceneObject::RemoveChild(std::unique_ptr<BSceneObject> const& Child)
{
    auto const Iter = std::find(std::begin(Children), std::end(Children), Child);
    if (Iter != std::end(Children)) {
        (*Iter)->SetParent(nullptr);
        Children.erase(Iter);
    }
}
