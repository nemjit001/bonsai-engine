#include "Filesystem.hpp"

#include "Core/PlatformDefines.hpp"

#if     BONSAI_PLATFORM_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
#endif //BONSAI_PLATFORM_WINDOWS

namespace Bonsai::Filesystem
{
    Path GetProgramLocation()
    {
    #if     BONSAI_PLATFORM_WINDOWS
        CHAR path[MAX_PATH]{};
        GetModuleFileNameA(nullptr, path, sizeof(path) - 1);
        return std::filesystem::weakly_canonical(path);
    #elif   BONSAI_PLATFORM_LINUX
        Path path("/proc/self/exe");
        return std::filesystem::weakly_canonical(path);
    #else
        return {};
    #endif
    }

    Path GetProgramDirectory()
    {
        Path const location = GetProgramLocation();
        if (!location.has_parent_path()) {
            return {}; // Return empty path
        }

        // Return parent path
        return location.parent_path();
    }
} // namespace Bonsai::Filesystem
