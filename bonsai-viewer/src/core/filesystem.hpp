#pragma once
#ifndef BONSAI_VIEWER_CORE_FILESYSTEM_HPP
#define BONSAI_VIEWER_CORE_FILESYSTEM_HPP

#include <filesystem>
#include <string>

namespace bonsai::core
{
    using Path = std::filesystem::path;

    /// @brief The filesystem class is used for basic file system operations.
    class FileSystem
    {
    public:
        /// @brief Create a canonical path from a given path.
        /// @param path 
        /// @return 
        static Path canonical(Path const& path);

        /// @brief Get the directory for this program.
        /// @return 
        static Path program_dir();

        /// @brief Read a file from a given path.
        /// @param path 
        /// @return 
        static std::string read_file(Path const& path);
    };
} // namespace bonsai::core

#endif //BONSAI_VIEWER_CORE_FILESYSTEM_HPP
