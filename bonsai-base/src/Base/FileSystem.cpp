#include "FileSystem.hpp"

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
} // namespace fs
