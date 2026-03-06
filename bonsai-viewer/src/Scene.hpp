#pragma once
#ifndef SCENE_HPP
#define SCENE_HPP

#include <memory>

class BSceneObject;

/// @brief The Scene stores scene data for Bonsai. It is a tree structure that models node's parent-child relationships.
class BScene
{
public:
    BScene();
    ~BScene();

    BScene(BScene const&) = delete;
    BScene& operator=(BScene const&) = delete;

    BScene(BScene&&) = delete;
    BScene& operator=(BScene&&) = delete;

    BSceneObject* GetRoot() { return Root.get(); }
    BSceneObject* GetRoot() const { return Root.get(); }

private:
    std::unique_ptr<BSceneObject> Root;
};

#endif //SCENE_HPP
