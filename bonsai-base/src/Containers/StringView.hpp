#pragma once
#ifndef STRING_VIEW_HPP
#define STRING_VIEW_HPP

#include <string_view>

using namespace std::literals::string_view_literals; // Exporting the namespace might not be optimal, but it allows for some convenient user-defined literal access.
using StringView = std::string_view;

#endif //STRING_VIEW_HPP
