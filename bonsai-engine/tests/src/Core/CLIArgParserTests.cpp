#include <gtest/gtest.h>
#include <BonsaiCore.hpp>

TEST(CLIArgParserTests, TestEmptyArgs)
{
    CLIArgParser args{};
    EXPECT_FALSE(args.IsSet("-foo"));
    EXPECT_FALSE(args.GetValue("--foo").has_value());
    EXPECT_FALSE(args.GetLastArg().has_value());
}

TEST(CLIArgParserTests, TestFlagIsSet)
{
    constexpr char const* argv[] = {
        "dummy_executable_program",
        "-foo-flag",
    };
    constexpr int argc = std::size(argv);

    CLIArgParser args(argc, argv);
    EXPECT_TRUE(args.IsSet("-foo-flag"));
}

TEST(CLIArgParserTests, TestOptionValueIsSet)
{
    constexpr char const* argv[] = {
        "dummy_executable_program",
        "--foo-option",
        "42",
    };
    constexpr int argc = std::size(argv);

    CLIArgParser args(argc, argv);
    EXPECT_EQ(args.GetValue("--foo-option"), "42");
}

TEST(CLIArgParserTests, TestGetLastArg)
{
    constexpr char const* argv[] = {
        "dummy_executable_program",
        "--foo-option",
        "42",
        "the_last_arg",
    };
    constexpr int argc = std::size(argv);

    CLIArgParser args(argc, argv);
    EXPECT_EQ(args.GetLastArg(), "the_last_arg");
}
