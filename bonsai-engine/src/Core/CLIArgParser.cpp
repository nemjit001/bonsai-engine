#include "CLIArgParser.hpp"

CLIArgParser::CLIArgParser(IAllocator* allocator)
{
    //
}

CLIArgParser& CLIArgParser::AddFlag(char name, char const* longName)
{
    CLIArgFlag flag{};
    flag.name = name;
    flag.longName = longName ? String(longName) : String();
    flag.isSet = false;

    _flags.push_back(flag);
    return *this;
}

CLIArgParser& CLIArgParser::AddOption(char const* name)
{
    CLIArgOption option{};
    option.name = name ? String(name) : String();
    option.value = {};

    _options.push_back(option);
    return *this;
}

bool CLIArgParser::Parse(int argc, char const* const* argv)
{
    for (int i = 0; i < argc; i++)
    {
        char const* arg = argv[i];

        // TODO(nemjit001): Handle end of args
        if (std::strcmp(arg, "--") == 0) {
            break;
        }

        // Parse args
        if (std::strstr(arg, "-") == arg)
        {
            if (std::strstr(arg, "--") == arg)
            {
                char const* argName = &arg[2]; // Skip first 2 dashes to get start of name

                // Try to parse as long flag
                bool found = false;
                for (auto& flag : _flags)
                {
                    if (flag.longName == argName)
                    {
                        found = true;
                        flag.isSet = true;
                    }
                }
                
                // Try to parse as option
                for (auto& option : _options)
                {
                    if (option.name == argName)
                    {
                        found = true;
                        if (i + 1 < argc) {
                            option.value = argv[i + 1];
                        }
                    }
                }

                if (!found) {
                    return false;
                }
            }
            else
            {
                // Parse as short flag char sequence
                char const* flagArg = &arg[1]; // Skip first dash to get start of short flags
                for (; *flagArg != '\0'; flagArg++)
                {
                    bool found = false;
                    for (auto& flag : _flags)
                    {
                        if (flag.name == *flagArg)
                        {
                            found = true;
                            flag.isSet = true;
                        }
                    }

                    if (!found) {
                        return false;
                    }
                }
            }
        }
        else
        {
            // Skip non-option, non-flag arg
            continue;
        }
    }

    return true;
}

bool CLIArgParser::IsSet(char name) const
{
    for (auto const& flag : _flags)
    {
        if (flag.name == name) {
            return flag.isSet;
        }
    }

    return false;
}

std::optional<String> CLIArgParser::GetValue(char const* name) const
{
    for (auto const& option : _options)
    {
        if (option.name == name && !option.value.empty())
        {
            return option.value;
        }
    }

    return {};
}
