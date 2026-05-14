cmake_minimum_required(VERSION 3.28)
project("Bonsai Engine")
enable_testing()

# Options
option(BUILD_TESTS "Enable unit tests" ON)

# Add CMake utils
include(cmake/utils.cmake)

# add sub projects
add_subdirectory(external)
add_subdirectory(lib)
add_subdirectory(bin)
