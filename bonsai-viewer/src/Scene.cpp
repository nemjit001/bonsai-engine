#include "Scene.hpp"

#include "SceneObject.hpp"

BScene::BScene()
    :
    Root(std::make_unique<BSceneObject>("Root"))
{
    //
}

BScene::~BScene() = default;
