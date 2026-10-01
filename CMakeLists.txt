cmake_minimum_required(VERSION 3.28)
project("Bonsai Engine" VERSION 0.1.0)

# Include cmake helpers
include(cmake/dependencies.cmake)
include(cmake/helpers.cmake)

# Find dependencies
find_package(SDL3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

# Set up targets
add_executable(BonsaiEngine)
target_copy_runtime_dlls(BonsaiEngine)
target_extended_warnings(BonsaiEngine)
target_sources(BonsaiEngine
    PRIVATE
        src/Application.cpp
        src/Application.hpp
        src/Logger.cpp
        src/Logger.hpp
        src/PlatformDefines.hpp
        src/main.cpp
)
target_compile_features(BonsaiEngine
    PRIVATE
        cxx_std_17
)
target_link_libraries(BonsaiEngine
    PRIVATE
        SDL3::SDL3
        spdlog::spdlog
)
