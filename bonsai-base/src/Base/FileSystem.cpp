#include "FileSystem.hpp"

#include <fstream>
#include "PlatformDefines.hpp"

#if     BONSAI_PLATFORM_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
#endif  //BONSAI_PLATFORM_WINDOWS

namespace fs
{
    Path GetProgramLocation()
    {
    #if     BONSAI_PLATFORM_WINDOWS
        char path[MAX_PATH]{};
        GetModuleFileName(nullptr, path, MAX_PATH - 1);
        return std::filesystem::weakly_canonical(Path(path));
    #elif   BONSAI_PLATFORM_LINUX
        Path const executablePath("/proc/self/exe");
        return std::filesystem::weakly_canonical(executablePath);
    #else
        return {}; // Return an empty path when unknown.
    #endif
    }

    Path GetProgramDirectory()
    {
        Path const executable = GetProgramLocation();
        if (executable.has_parent_path()) {
            return executable.parent_path();
        }

        return {}; // Return an empty path when unknown
    }

    String ReadFile(Path const& path, IAllocator* allocator)
    {
        // Try to open file
        std::ifstream file(path, std::ios::ate);
        if (!file) {
            return {};
        }

        // Size output string based on file contents
        usize const numCharacters = file.tellg();
        String contents(ResolveAllocator(allocator));
        contents.resize(numCharacters, '\0');

        // Read file contents into string
        file.seekg(std::ios::beg);
        file.read(&contents[0], numCharacters);

        // Done
        return contents;
    }

    DynamicArray<uint8> ReadBinaryFile(Path const& path, IAllocator* allocator)
    {
        // Try to open file
        std::ifstream file(path, std::ios::ate | std::ios::binary);
        if (!file) {
            return {};
        }

        // Size output string based on file contents
        usize const bytes = file.tellg();
        DynamicArray<uint8> contents(ResolveAllocator(allocator));
        contents.resize(bytes, '\0');

        // Read file contents into string
        file.seekg(std::ios::beg);
        file.read(reinterpret_cast<char*>(&contents[0]), bytes);

        // Done
        return contents;
    }
} // namespace fs
