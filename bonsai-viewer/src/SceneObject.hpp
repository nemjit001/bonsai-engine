#pragma once
#ifndef SCENE_OBJECT_HPP
#define SCENE_OBJECT_HPP

#include <memory>
#include <string>
#include <vector>

/// @brief A SceneObject represents an entity in the virtual scene.
class BSceneObject
{
public:
    BSceneObject() = default;
    BSceneObject(std::string const& ObjectName);
    ~BSceneObject() = default;

    BSceneObject(BSceneObject const&) = delete;
    BSceneObject& operator=(BSceneObject const&) = delete;

    BSceneObject(BSceneObject&&) = delete;
    BSceneObject& operator=(BSceneObject&&) = delete;

    /// @brief Add a child SceneObject.
    /// @param Child 
    void AddChild(std::unique_ptr<BSceneObject>&& Child);

    /// @brief Remove a child scene object.
    /// @param Child 
    void RemoveChild(std::unique_ptr<BSceneObject> const& Child);

    /// @brief Get the children SceneObjects.
    /// @return 
    std::vector<std::unique_ptr<BSceneObject>> const& GetChildren() const { return Children; }

    /// @brief Set the parent SceneObject.
    /// @param NewParent 
    void SetParent(BSceneObject* NewParent);

    /// @brief Get the parent SceneObject.
    /// @return 
    BSceneObject* GetParent() const { return Parent; }

    /// @brief Set the name for the SceneObject.
    /// @return 
    std::string GetName() const { return Name; }

private:
    std::string Name = "Scene Object";
    BSceneObject* Parent = nullptr;
    std::vector<std::unique_ptr<BSceneObject>> Children;
};

#endif //SCENE_OBJECT_HPP
