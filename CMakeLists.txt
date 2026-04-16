cmake_minimum_required(VERSION 3.25)
project("Bonsai Engine")
enable_testing()

# Options
option(BUILD_TESTS "Enable unit tests" ON)

# Add CMake utils
include(cmake/utils.cmake)
