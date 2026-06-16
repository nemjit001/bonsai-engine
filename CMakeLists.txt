cmake_minimum_required(VERSION 3.28)
project("Bonsai Engine")
enable_testing()

# Options
option(BUILD_TESTS "Enable unit tests" ON)

# Set up output dirs
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/$<CONFIG>")

# Add CMake utils
include(cmake/utils.cmake)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# add sub projects
add_subdirectory(external)
add_subdirectory(bonsai-base)
add_subdirectory(bonsai-engine)
