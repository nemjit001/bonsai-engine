#pragma once
#ifndef BONSAI_VIEWER_CORE_FILESYSTEM_HPP
#define BONSAI_VIEWER_CORE_FILESYSTEM_HPP

#include <filesystem>

namespace bonsai
{
    using Path = std::filesystem::path;

    /// @brief The filesystem class is used for basic file system operations.
    class FileSystem
    {
    public:
        /// @brief Create a canonical path from a path type.
        /// @param path 
        /// @return 
        static Path canonical(Path const& path);

        /// @brief Get the directory for this program.
        /// @return 
        static Path program_dir();
    };
} // namespace bonsai

#endif //BONSAI_VIEWER_CORE_FILESYSTEM_HPP
