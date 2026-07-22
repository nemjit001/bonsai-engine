#pragma once
#ifndef FILE_SYSTEM_HPP
#define FILE_SYSTEM_HPP

#include <filesystem>
#include "Allocator.hpp"
#include "PlatformTypes.hpp"
#include "Containers/DynamicArray.hpp"
#include "Containers/String.hpp"

namespace fs
{
    using Path = std::filesystem::path;

    /// @brief Get the path to the program binary.
    /// @return The path to the program binary.
    Path GetProgramLocation();

    /// @brief Get the directory where the program binary is located.
    /// @return The directory of the program binary.
    Path GetProgramDirectory();

    /// @brief Read a text file from disk.
    /// @param path Path to the file.
    /// @param allocator Optional allocator to use for the file contents.
    /// @return A string with the file contents, empty on failure.
    String ReadFile(Path const& path, IAllocator* allocator = nullptr);

    /// @brief Read a binary file from disk.
    /// @param path Path to the binary file.
    /// @param allocator Optional allocator to use for the file contents.
    /// @return A dynamic array with the file contents, empty on failure.
    DynamicArray<uint8> ReadBinaryFile(Path const& path, IAllocator* allocator = nullptr);
} // namespace fs

#endif //FILE_SYSTEM_HPP
