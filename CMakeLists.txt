cmake_minimum_required(VERSION 3.25)
project("Bonsai Renderer")

# Options
option(BUILD_TESTS "Enable unit tests" ON)

# Add CMake utils
include(cmake/utils.cmake)

# Configure default CMake target behaviour
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (BUILD_TESTS)
    enable_testing()
endif()

# Configure sub projects
add_subdirectory(bonsai-viewer)
