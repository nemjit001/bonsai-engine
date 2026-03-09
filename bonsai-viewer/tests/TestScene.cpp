#include <gtest/gtest.h>
#include "Scene.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Light.hpp"
#include "Scene/RenderableObject.hpp"
#include "Scene/SceneObject.hpp"

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
    ASSERT_EQ(Children[0]->GetSceneObjectType(), SOType_SceneObject);
}

TEST(SceneTests, TestCreateCamera)
{
    BScene Scene{};
    BSceneObject* Root = Scene.GetRoot();
    auto Child = std::make_unique<BCamera>("Camera");
    Root->AddChild(std::move(Child));

    std::vector<std::unique_ptr<BSceneObject>> const& Children = Root->GetChildren();
    ASSERT_EQ(Children.size(), 1);
    ASSERT_EQ(Children[0]->GetParent(), Root);
    ASSERT_EQ(Children[0]->GetName(), "Camera");
    ASSERT_EQ(Children[0]->GetSceneObjectType(), SOType_Camera);
}

TEST(SceneTests, TestCreateLight)
{
    BScene Scene{};
    BSceneObject* Root = Scene.GetRoot();
    auto Child = std::make_unique<BLight>("Light");
    Root->AddChild(std::move(Child));

    std::vector<std::unique_ptr<BSceneObject>> const& Children = Root->GetChildren();
    ASSERT_EQ(Children.size(), 1);
    ASSERT_EQ(Children[0]->GetParent(), Root);
    ASSERT_EQ(Children[0]->GetName(), "Light");
    ASSERT_EQ(Children[0]->GetSceneObjectType(), SOType_Light);
}

TEST(SceneTests, TestCreateRenderObject)
{
    BScene Scene{};
    BSceneObject* Root = Scene.GetRoot();
    auto Child = std::make_unique<BRenderableObject>("Renderable");
    Root->AddChild(std::move(Child));

    std::vector<std::unique_ptr<BSceneObject>> const& Children = Root->GetChildren();
    ASSERT_EQ(Children.size(), 1);
    ASSERT_EQ(Children[0]->GetParent(), Root);
    ASSERT_EQ(Children[0]->GetName(), "Renderable");
    ASSERT_EQ(Children[0]->GetSceneObjectType(), SOType_Renderable);
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
