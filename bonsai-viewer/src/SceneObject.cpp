#include "SceneObject.hpp"

#include <algorithm>

BSceneObject::BSceneObject(std::string const& ObjectName)
    :
    Name(ObjectName),
    Parent(nullptr),
    Children()
{
    //
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

void BSceneObject::SetParent(BSceneObject* NewParent)
{
    Parent = NewParent;
}
