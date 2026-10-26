#include <gtest/gtest.h>
#include "Camera/Camera.hpp"

TEST(CameraTests, TestCreatePerspectiveCamera)
{
    Bonsai::CameraPerspectiveParameters parameters{};
    parameters.horizontalFOV = 90.0F;
    parameters.aspectRatio = 16.0F / 9.0F;

    Bonsai::Camera camera(parameters);
    EXPECT_EQ(camera.GetCameraType(), Bonsai::CameraType::Perspective);
    EXPECT_NE(camera.GetProjectionMatrix(), glm::identity<glm::mat4>());
}

TEST(CameraTests, TestCreateOrthographicCamera)
{
    Bonsai::CameraOrthographicParameters parameters{};
    parameters.yMag = 10.0F;
    parameters.aspectRatio = 1.0F;

    Bonsai::Camera camera(parameters);
    EXPECT_EQ(camera.GetCameraType(), Bonsai::CameraType::Orthographic);
    EXPECT_NE(camera.GetProjectionMatrix(), glm::identity<glm::mat4>());
}
