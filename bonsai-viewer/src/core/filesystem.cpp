#include "filesystem.hpp"

#include <fstream>

#if _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
#endif

namespace bonsai::core
{
    Path FileSystem::canonical(Path const& path)
    {
        return std::filesystem::weakly_canonical(path);
    }

    Path FileSystem::program_dir()
    {
        Path executable_path{};
    #if     _WIN32
        char filename[MAX_PATH]{};
        GetModuleFileName(nullptr, filename, MAX_PATH);

        executable_path = Path(filename);
    #elif   __unix__
        executable_path = Path("/proc/self/exe");
        executable_path = FileSystem::canonical(executable_path); // Resolves symlinks
    #endif

        return executable_path.parent_path();
    }

    std::string FileSystem::read_file(Path const& path)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file)
        {
            return {};
        }

        file.seekg(std::ios::end);
        size_t const bytes = file.tellg();
        std::string buffer(bytes, '\0');

        file.seekg(std::ios::beg);
        file.read(&buffer[0], bytes);
        file.close();

        return buffer;
    }
} // namespace bonsai::core

