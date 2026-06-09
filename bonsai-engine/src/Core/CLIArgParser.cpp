#include "CLIArgParser.hpp"

#include <algorithm>

CLIArgParser::CLIArgParser(IAllocator& allocator, int argc, char const* const* argv)
    :
    _cliArgs(&allocator)
{
    _cliArgs.reserve(argc);
    PrintDebugString("CLIArgParser: Got %zu cli argument(s)", argc);
    for (int i = 0; i < argc; i++)
    {
        PrintDebugString("  [%d] %s", i, argv[i]);
        _cliArgs.push_back(String(argv[i], &allocator));
    }
}

bool CLIArgParser::IsSet(String const& option) const
{
    auto const iter = std::find(_cliArgs.begin(), _cliArgs.end(), option);
    return iter != _cliArgs.end();
}

std::optional<String> CLIArgParser::GetValue(String const& option) const
{
    for (usize i = 0; i < _cliArgs.size(); i++)
    {
        auto const& value = _cliArgs[i];
        if (value == option && (i + 1) < _cliArgs.size()) {
            return _cliArgs[i + 1];
        }
    }

    return {};
}

std::optional<String> CLIArgParser::GetLastArg() const
{
    if (!_cliArgs.empty()) {
        return _cliArgs.back();
    }

    return {};
}
