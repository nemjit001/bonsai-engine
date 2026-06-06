cmake_minimum_required(VERSION 3.28)
project("Bonsai Engine")
enable_testing()

# Options
option(BUILD_TESTS "Enable unit tests" ON)

# Add CMake utils
include(cmake/utils.cmake)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# add sub projects
add_subdirectory(external)
add_subdirectory(bonsai-engine)
