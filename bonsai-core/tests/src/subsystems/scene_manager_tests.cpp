#include <gtest/gtest.h>
#include <bonsai/subsystems/scene_manager.hpp>

TEST(SceneManagerTests, TestInitSceneManager)
{
    auto scene_manager = bonsai::subsystems::SceneManager();
    ASSERT_TRUE(scene_manager.init({}));
    scene_manager.shutdown();
}

TEST(SceneManagerTests, TestDefaultSceneInit)
{
    auto scene_manager = bonsai::subsystems::SceneManager();
    ASSERT_TRUE(scene_manager.init({}));

    bonsai::assets::Scene& scene = scene_manager.active_scene();
    ASSERT_EQ(scene.name, "Bonsai Scene");

    scene_manager.shutdown();
}
