include(FetchContent)

# Cereal
set(BUILD_SANDBOX OFF CACHE BOOL "" FORCE)
set(SKIP_PERFORMANCE_COMPARISON ON CACHE BOOL "" FORCE)
FetchContent_Declare(Cereal
    GIT_REPOSITORY  https://github.com/USCiLab/cereal.git
    GIT_TAG         v1.3.2
    GIT_SHALLOW     TRUE
    OVERRIDE_FIND_PACKAGE
)

# GLM
FetchContent_Declare(glm
    GIT_REPOSITORY  https://github.com/g-truc/glm.git
    GIT_TAG         1.0.3
    GIT_SHALLOW     TRUE
    OVERRIDE_FIND_PACKAGE
)

# EnTT
FetchContent_Declare(EnTT
    GIT_REPOSITORY  https://github.com/skypjack/entt.git
    GIT_TAG         v4.0.0
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

# spdlog
FetchContent_Declare(spdlog
    GIT_REPOSITORY  https://github.com/gabime/spdlog.git
    GIT_TAG         v1.17.0
    GIT_SHALLOW     TRUE
    OVERRIDE_FIND_PACKAGE
)
