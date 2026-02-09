#include <gtest/gtest.h>
#include <bonsai/subsystems/window_system.hpp>

namespace bonsai::subsystems
{
    TEST(WindowSystemTests, TestInitSystem)
    {
        WindowSystem windowing_system{};
        ASSERT_TRUE(windowing_system.init());
        windowing_system.shutdown();
    }

    TEST(WindowSystemTests, TestRunSystem)
    {
        WindowSystem windowing_system{};
        ASSERT_TRUE(windowing_system.init());
        windowing_system.run();
        windowing_system.shutdown();
    }

    TEST(WindowSystemTests, TestCreateWindow)
    {
        WindowSystem windowing_system{};
        ASSERT_TRUE(windowing_system.init());

        WindowID test_window = windowing_system.create_window("Test Window", 512, 512);
        EXPECT_NE(INVALID_WINDOW_ID, test_window);

        windowing_system.run();

        windowing_system.destroy_window(test_window);
        windowing_system.shutdown();
    }
} // namespace bonsai::subsystems
