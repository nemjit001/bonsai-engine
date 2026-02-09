#include <bonsai/core/filesystem.hpp>

int main(int, char*[])
{
    // Check executable location
    bonsai::core::Path const program_dir = bonsai::core::FileSystem::program_dir();
    printf("Running from \"%s\"\n", program_dir.string().c_str());

    return 0;
}
