#include <gtest/gtest.h>
#include <BonsaiBase.hpp>

TEST(FileSystemTests, TestGetProgramLocation)
{
    fs::Path const location = fs::GetProgramLocation();
    ASSERT_FALSE(location.empty());
    ASSERT_TRUE(location.has_parent_path());
    ASSERT_EQ(location.parent_path(), fs::GetProgramDirectory());
}

TEST(FileSystemTests, TestGetProgramDirectoryNeverEmpty)
{
    ASSERT_FALSE(fs::GetProgramDirectory().empty());
}
