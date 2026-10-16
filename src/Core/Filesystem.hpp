#pragma once
#ifndef BONSAI_FILESYSTEM_HPP
#define BONSAI_FILESYSTEM_HPP

#include <filesystem>

namespace Bonsai::Filesystem
{
    using Path = std::filesystem::path;

    /// @brief Get the running program location in the filesystem.
    /// @return The fully qualified path to the program location.
    Path GetProgramLocation();

    /// @brief Get the running program's directory in the filesystem.
    /// @return The fully qualified path to the program's parent directory.
    Path GetProgramDirectory();
} // namespace Bonsai::Filesystem

#endif //BONSAI_FILESYSTEM_HPP
