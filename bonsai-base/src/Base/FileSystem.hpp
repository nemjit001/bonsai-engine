#pragma once
#ifndef FILE_SYSTEM_HPP
#define FILE_SYSTEM_HPP

#include <filesystem>

namespace fs
{
    using Path = std::filesystem::path;

    /// @brief Get the path to the program binary.
    /// @return The path to the program binary.
    Path GetProgramLocation();

    /// @brief Get the directory where the program binary is located.
    /// @return The directory of the program binary.
    Path GetProgramDirectory();
} // namespace fs

#endif //FILE_SYSTEM_HPP
