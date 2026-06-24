#pragma once
#ifndef CLI_ARG_PARSER_HPP
#define CLI_ARG_PARSER_HPP

#include <optional>
#include <BonsaiBase.hpp>
#include <Containers/DynamicArray.hpp>
#include <Containers/String.hpp>

/// @brief CLI argument flag with support for short and long names.
struct CLIArgFlag
{
    char name;
    std::optional<String> longName;
    bool isSet;
};

/// @brief CLI argument option key/value pair.
struct CLIArgOption
{
    String name;
    std::optional<String> value;
};

/// @brief The CLIArgParser provides a basic interface for testing CLI argument availability.
class CLIArgParser
{
public:
    /// @brief Create a new CLIArgParser.
    /// @param allocator Optional allocator to use for internal allocations.
    CLIArgParser(IAllocator* allocator = nullptr);

    /// @brief Add a flag to the CLI arguments.
    /// @param name Name for the flag.
    /// @param longName Optional longer name for the flag.
    /// @return 
    CLIArgParser& AddFlag(char name, char const* longName = nullptr);

    /// @brief Add an option to the CLI arguments.
    /// @param name Name for the option.
    /// @return 
    CLIArgParser& AddOption(char const* name);

    /// @brief Parse the CLI arguments vector.
    /// @param argc Number of CLI arguments in the vector.
    /// @param argv Arguments vector.
    /// @return A boolean indicating successful parsing.
    [[nodiscard]] bool Parse(int argc, char const* const* argv);

    /// @brief Check if a flag was set in the argument list.
    /// @param name Flag name.
    /// @return A boolean indicating flag availability.
    [[nodiscard]] bool IsSet(char name) const;

    /// @brief Try to get a set option value from the argument list.
    /// @param name Option name.
    /// @return An optional string value.
    [[nodiscard]] std::optional<String> GetValue(char const* name) const;

private:
    IAllocator* _allocator = nullptr;
    DynamicArray<CLIArgFlag> _flags;
    DynamicArray<CLIArgOption> _options;
};

#endif //CLI_ARG_PARSER_HPP
