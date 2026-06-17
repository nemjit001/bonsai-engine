#include <gtest/gtest.h>
#include <Core.hpp>

TEST(CLIArgParserTests, TestEmptyArgs)
{
    CLIArgParser args;
    args.AddFlag('f')
        .AddOption("bar");

    EXPECT_TRUE(args.Parse(0, nullptr));
    EXPECT_FALSE(args.IsSet('f'));
    EXPECT_FALSE(args.GetValue("bar").has_value());
}

TEST(CLIArgParserTests, TestShortFlagIsSet)
{
    constexpr char const* argv[] = {
        "dummy_executable_program",
        "-fb",
    };
    constexpr int argc = std::size(argv);

    DefaultAllocator allocator{};
    CLIArgParser args(&allocator);
    args.AddFlag('f', "foo-flag")
        .AddFlag('b', "bar-flag");
    
    EXPECT_TRUE(args.Parse(argc, argv));
    EXPECT_TRUE(args.IsSet('f'));
    EXPECT_TRUE(args.IsSet('b'));
}

TEST(CLIArgParserTests, TestLongFlagIsSet)
{
    constexpr char const* argv[] = {
        "dummy_executable_program",
        "--foo-flag",
    };
    constexpr int argc = std::size(argv);

    DefaultAllocator allocator{};
    CLIArgParser args(&allocator);
    args.AddFlag('f', "foo-flag");
    
    EXPECT_TRUE(args.Parse(argc, argv));
    EXPECT_TRUE(args.IsSet('f'));
}

TEST(CLIArgParserTests, TestOptionValueIsSet)
{
    constexpr char const* argv[] = {
        "dummy_executable_program",
        "--foo-option",
        "42",
    };
    constexpr int argc = std::size(argv);

    DefaultAllocator allocator{};
    CLIArgParser args(&allocator);
    args.AddOption("foo-option");

    EXPECT_TRUE(args.Parse(argc, argv));
    EXPECT_EQ(args.GetValue("foo-option"), "42");
}

TEST(CLIArgParserTests, TestExtraArgs)
{
    constexpr char const* argv[] = {
        "dummy_executable_program",
        "--foo-option",
        "42",
        "--",
        "--extra",
        "--arg",
    };
    constexpr int argc = std::size(argv);

    DefaultAllocator allocator{};
    CLIArgParser args(&allocator);
    args.AddOption("foo-option")
        .AddFlag('e', "extra")
        .AddFlag('a', "arg");

    EXPECT_TRUE(args.Parse(argc, argv));
    EXPECT_EQ(args.GetValue("foo-option"), "42");
    EXPECT_FALSE(args.IsSet('e'));
    EXPECT_FALSE(args.IsSet('a'));
}

TEST(CLIArgParserTests, TestInvalidArgs)
{
    constexpr char const* argv[] = {
        "dummy_executable_program",
        "--invalid",
    };
    constexpr int argc = std::size(argv);

    DefaultAllocator allocator{};
    CLIArgParser args(&allocator);
    args.AddFlag('v', "valid");

    EXPECT_FALSE(args.Parse(argc, argv));
}
