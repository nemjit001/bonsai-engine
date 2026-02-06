#include "filesystem.hpp"

#if _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
#endif

namespace bonsai
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
} // namespace bonsai

