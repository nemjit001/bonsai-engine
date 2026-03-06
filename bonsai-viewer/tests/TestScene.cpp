#include <gtest/gtest.h>
#include "Scene.hpp"
#include "SceneObject.hpp"

TEST(SceneTests, TestCreateEmptyScene)
{
    BScene Scene{};
    ASSERT_EQ(Scene.GetRoot()->GetName(), "Root");
}

TEST(SceneTests, TestCreateSceneObject)
{
    BScene Scene{};
    BSceneObject* Root = Scene.GetRoot();
    auto Child = std::make_unique<BSceneObject>("Child");
    Root->AddChild(std::move(Child));

    std::vector<std::unique_ptr<BSceneObject>> const& Children = Root->GetChildren();
    ASSERT_EQ(Children.size(), 1);
    ASSERT_EQ(Children[0]->GetParent(), Root);
    ASSERT_EQ(Children[0]->GetName(), "Child");
}

TEST(SceneTests, TestRemoveSceneObject)
{
    BScene Scene{};
    BSceneObject* Root = Scene.GetRoot();
    auto Child = std::make_unique<BSceneObject>("Child");
    Root->AddChild(std::move(Child));

    std::vector<std::unique_ptr<BSceneObject>> const& Children = Root->GetChildren();
    Root->RemoveChild(Children[0]);
    ASSERT_EQ(Children.size(), 0);
}
