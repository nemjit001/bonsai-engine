#include <gtest/gtest.h>
#include <bonsai/subsystems/logging_manager.hpp>

namespace bonsai::subsystems
{
    TEST(LoggingManagerTests, TestInitSystem)
    {
        auto& logging_system = LoggingManager::get();
        ASSERT_TRUE(logging_system.init());
        logging_system.shutdown();
    }
} // namespace bonsai::subsystems
