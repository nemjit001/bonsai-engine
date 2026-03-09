#pragma once
#ifndef SCENE_OBJECT_HPP
#define SCENE_OBJECT_HPP

#include <memory>
#include <string>
#include <vector>
#include "Transform.hpp"

/// @brief A SceneObject represents an entity in the virtual scene.
class BSceneObject
{
public:
    BSceneObject();
    BSceneObject(std::string const& ObjectName);
    virtual ~BSceneObject() = default;

    BSceneObject(BSceneObject const&) = delete;
    BSceneObject& operator=(BSceneObject const&) = delete;

    BSceneObject(BSceneObject&&) = default;
    BSceneObject& operator=(BSceneObject&&) = default;

    /// @brief Set the name for the SceneObject.
    /// @return 
    std::string GetName() const { return Name; }

    /// @brief Set the transform for this SceneObject.
    /// @param NewTransform 
    void SetTransform(BTransform const& NewTransform);

    /// @brief Get the transform for this SceneObject.
    /// @return 
    BTransform GetTransform() const { return Transform; }

    /// @brief Get the world-space transform matrix for the SceneObject.
    /// @return 
    glm::mat4 GetWorldMatrix() const;

    /// @brief Set the parent SceneObject.
    /// @param NewParent 
    void SetParent(BSceneObject* NewParent);

    /// @brief Get the parent SceneObject.
    /// @return 
    BSceneObject* GetParent() const { return Parent; }

    /// @brief Add a child SceneObject.
    /// @param Child 
    void AddChild(std::unique_ptr<BSceneObject>&& Child);

    /// @brief Remove a child scene object.
    /// @param Child 
    void RemoveChild(std::unique_ptr<BSceneObject> const& Child);

    /// @brief Get the children SceneObjects.
    /// @return 
    std::vector<std::unique_ptr<BSceneObject>> const& GetChildren() const { return Children; }

private:
    std::string Name;
    BTransform Transform;
    BSceneObject* Parent;
    std::vector<std::unique_ptr<BSceneObject>> Children;
};

#endif //SCENE_OBJECT_HPP
