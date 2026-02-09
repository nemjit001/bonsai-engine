#include <gtest/gtest.h>
#include <bonsai/subsystems/logging_system.hpp>

namespace bonsai::subsystems
{
    TEST(LoggingSystemTests, TestInitSystem)
    {
        LoggingSystem logging_system{};
        ASSERT_TRUE(logging_system.init());
        logging_system.shutdown();
    }
} // namespace bonsai::subsystems
