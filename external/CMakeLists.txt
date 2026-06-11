message(STATUS "Setting up external dependencies")

# Set up dependencies
include(FetchContent)

FetchContent_Declare(GTest
        GIT_REPOSITORY  "https://github.com/google/googletest.git"
        GIT_TAG         "v1.17.0"
        GIT_SHALLOW     TRUE
        GIT_PROGRESS    TRUE
        OVERRIDE_FIND_PACKAGE
        EXCLUDE_FROM_ALL
)

FetchContent_Declare(SDL3
        GIT_REPOSITORY  "https://github.com/libsdl-org/SDL.git"
        GIT_TAG         "release-3.4.x"
        GIT_SHALLOW     TRUE
        GIT_PROGRESS    TRUE
        OVERRIDE_FIND_PACKAGE
        EXCLUDE_FROM_ALL
)

FetchContent_Declare(WebGPU
        GIT_REPOSITORY  "https://github.com/eliemichel/WebGPU-distribution.git"
        GIT_TAG         "main"
        GIT_SHALLOW     TRUE
        GIT_PROGRESS    TRUE
        OVERRIDE_FIND_PACKAGE
        EXCLUDE_FROM_ALL
)

FetchContent_Declare(SDL3WebGPU
        GIT_REPOSITORY  "https://github.com/eliemichel/sdl3webgpu"
        GIT_TAG         "main"
        GIT_SHALLOW     TRUE
        GIT_PROGRESS    TRUE
        OVERRIDE_FIND_PACKAGE
        EXCLUDE_FROM_ALL
)
