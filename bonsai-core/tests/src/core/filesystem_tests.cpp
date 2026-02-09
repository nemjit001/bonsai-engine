#include <gtest/gtest.h>
#include <bonsai/core/filesystem.hpp>
#include "test_config.hpp"

namespace bonsai::core
{
    TEST(FilesystemTests, TestGetProgramDir)
    {
        Path path = FileSystem::program_dir();
        path = path.parent_path(); // Ditch the config folder from the path
        EXPECT_EQ(path, Path(TEST_BINARY_DIR));
    }
} // namespace bonsai::core

