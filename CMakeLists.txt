cmake_minimum_required(VERSION 3.25)
project("Bonsai Renderer")
enable_testing()

# Options
option(BUILD_TESTS "Enable unit tests" ON)
option(BONSAI_VULKAN_SUPPORT "Enable Vulkan support" OFF)
option(BONSAI_DX12_SUPPORT "Enable DirectX12 support" OFF)

# Configure graphics backends for supported platforms
if (WIN32)
    set(BONSAI_VULKAN_SUPPORT ON CACHE BOOL "" FORCE)
    set(BONSAI_DX12_SUPPORT ON CACHE BOOL "" FORCE)
elseif(UNIX)
    set(BONSAI_VULKAN_SUPPORT ON CACHE BOOL "" FORCE)
    set(BONSAI_DX12_SUPPORT OFF CACHE BOOL "" FORCE)
endif()

# Add CMake utils
include(cmake/utils.cmake)

# Configure default CMake target behaviour
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add external dependencies
add_subdirectory(external EXCLUDE_FROM_ALL)

# Configure sub projects
add_subdirectory(bonsai-viewer)
