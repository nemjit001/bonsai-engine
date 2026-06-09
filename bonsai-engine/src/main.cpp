#include <BonsaiEngine.hpp>

#if     BONSAI_PLATFORM_WINDOWS
    #include <Windows.h>
#endif  //BONSAI_PLATFORM_WINDOWS

/// @brief Default bonsai engine entrypoint.
/// @param argc 
/// @param argv 
/// @return 
static int BonsaiEntrypoint([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    return BonsaiEngine()
        .Run();
}

#if     BONSAI_PLATFORM_WINDOWS
int WINAPI WinMain(
    [[maybe_unused]] HINSTANCE hInstance,
    [[maybe_unused]] HINSTANCE hPrevInstance,
    [[maybe_unused]] LPSTR lpCmdLine,
    [[maybe_unused]] int nShowCmd
) {
    return BonsaiEntrypoint(__argc, __argv);
}
#else
int main(int argc, char* argv) {
    return BonsaiEntrypoint(argc, argv);
}
#endif
