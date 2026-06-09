#pragma once
#ifndef CLI_ARG_PARSER_HPP
#define CLI_ARG_PARSER_HPP

#include <optional>
#include <BonsaiBase.hpp>
#include <Containers/DynamicArray.hpp>
#include <Containers/String.hpp>

/// @brief The CLIArgParser provides a basic interface for testing CLI argument availability.
class CLIArgParser
{
public:
    CLIArgParser() = default;
    CLIArgParser(IAllocator& allocator, int argc, char const* const* argv);

    /// @brief Check if a flag was set in the argument list.
    /// @param option Opion name.
    /// @return A boolean indicating flag availability.
    bool IsSet(String const& option) const;

    /// @brief Try to get a set option value from the argument list.
    /// @param option Option name.
    /// @return An optional string value.
    std::optional<String> GetValue(String const& option) const;

    /// @brief Try to get the last argument in the argument list.
    /// @return An optional string value.
    std::optional<String> GetLastArg() const;

private:
    DynamicArray<String> _cliArgs;
};

#endif //CLI_ARG_PARSER_HPP
