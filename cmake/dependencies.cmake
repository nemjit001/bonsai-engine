include(FetchContent)

# SDL3
FetchContent_Declare(SDL3
    GIT_REPOSITORY  https://github.com/libsdl-org/SDL.git
    GIT_TAG         release-3.4.x
    GIT_SHALLOW     TRUE
    OVERRIDE_FIND_PACKAGE
)
