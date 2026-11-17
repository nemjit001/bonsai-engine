include(FetchContent)

# GTest
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_Declare(GTest
    GIT_REPOSITORY  https://github.com/google/googletest.git
    GIT_TAG         v1.18.0
    GIT_SHALLOW     TRUE
    OVERRIDE_FIND_PACKAGE
)

# SDL3
FetchContent_Declare(SDL3
    GIT_REPOSITORY  https://github.com/libsdl-org/SDL.git
    GIT_TAG         release-3.4.x
    GIT_SHALLOW     TRUE
    OVERRIDE_FIND_PACKAGE
)

# volk
FetchContent_Declare(volk
    GIT_REPOSITORY  https://github.com/zeux/volk.git
    GIT_TAG         master
    GIT_SHALLOW     TRUE
    OVERRIDE_FIND_PACKAGE
)
