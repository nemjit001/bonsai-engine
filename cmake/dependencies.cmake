include(FetchContent)

# SDL3
FetchContent_Declare(SDL3
    GIT_REPOSITORY  https://github.com/libsdl-org/SDL.git
    GIT_TAG         release-3.4.x
    GIT_SHALLOW     TRUE
    OVERRIDE_FIND_PACKAGE
)

# spdlog
FetchContent_Declare(spdlog
    GIT_REPOSITORY  https://github.com/gabime/spdlog.git
    GIT_TAG         v1.17.0
    GIT_SHALLOW     TRUE
    OVERRIDE_FIND_PACKAGE
)
