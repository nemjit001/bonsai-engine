#include <gtest/gtest.h>
#include <BonsaiBase.hpp>

TEST(FileSystemTests, TestGetProgramLocation)
{
    fs::Path const location = fs::GetProgramLocation();
    ASSERT_FALSE(location.empty());
    ASSERT_TRUE(location.has_parent_path());
    EXPECT_EQ(location.parent_path(), fs::GetProgramDirectory());
}

TEST(FileSystemTests, TestGetProgramDirectoryNeverEmpty)
{
    EXPECT_FALSE(fs::GetProgramDirectory().empty());
}

TEST(FileSystemTests, TestReadTextFileFromDisk)
{
    fs::Path const path = fs::GetProgramDirectory()/"test_assets/test_files/lorem_ipsum.txt";
    String const contents = fs::ReadFile(path);
    ASSERT_FALSE(contents.empty());

    EXPECT_TRUE(contents.find("Lorem ipsum dolor sit amet.") != String::npos);
}

TEST(FileSystemTests, TestReadBinaryFileFromDisk)
{
    fs::Path const path = fs::GetProgramDirectory()/"test_assets/test_files/deadbeef.bin";
    DynamicArray<uint8> const contents = fs::ReadBinaryFile(path);
    ASSERT_FALSE(contents.empty());

    uint32 const& value = *reinterpret_cast<uint32 const*>(contents.data());
    EXPECT_EQ(value, 0xDEADBEEF);
}
