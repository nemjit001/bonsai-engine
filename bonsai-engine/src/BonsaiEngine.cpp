#include "BonsaiEngine.hpp"

#include "BonsaiEngineConfig.hpp" // auto-generated config file

static DefaultAllocator gDefaultAllocator = DefaultAllocator{}; // Default allocator for use by subsystems.

bool BonsaiEngine::Init()
{
    // Start engine initialization
    PrintDebugString("Initializing Bonsai Engine (v%s)", BONSAI_ENGINE_VERSION_STRING);
    PrintDebugString("Initialized Bonsai Engine!");
    return true;
}

int32 BonsaiEngine::Run(int argc, char* argv[])
{
    if (!Init()) {
        return BonsaiEngineResult_BadInit;
    }

    return BonsaiEngineResult_Ok;
}
