#include "Scene.hpp"

#include "Scene/SceneObject.hpp"

BScene::BScene()
    :
    Root(std::make_unique<BSceneObject>("Root"))
{
    //
}

BScene::~BScene() = default;
