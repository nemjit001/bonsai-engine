project("BonsaiBase"
    VERSION 0.0.1
    DESCRIPTION "Platform Independence Layer for the Bonsai Engine."
    LANGUAGES CXX
)

find_package(SDL3 CONFIG REQUIRED)
find_package(sdl3webgpu CONFIG REQUIRED)
find_package(WebGPU CONFIG REQUIRED)

add_library(BonsaiBase)
add_library(Bonsai::Base ALIAS BonsaiBase)
target_sources(BonsaiBase
    PRIVATE
        src/BonsaiBase.hpp
        src/Base/Allocator.cpp
        src/Base/Allocator.hpp
        src/Base/Assert.cpp
        src/Base/Assert.hpp
        src/Base/DebugOutput.cpp
        src/Base/DebugOutput.hpp
        src/Base/Memory.cpp
        src/Base/Memory.hpp
        src/Base/PlatformDefines.hpp
        src/Base/PlatformTypes.hpp
        src/Containers/DynamicArray.hpp
        src/Containers/HashMap.hpp
        src/Containers/StaticArray.hpp
        src/Containers/String.hpp
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS src/
    FILES
        src/BonsaiBase.hpp
        src/Containers/DynamicArray.hpp
        src/Containers/HashMap.hpp
        src/Containers/StaticArray.hpp
        src/Containers/String.hpp
)
target_compile_features(BonsaiBase PRIVATE cxx_std_17)
target_enable_extended_warnings(BonsaiBase)
target_link_libraries(BonsaiBase
    PUBLIC
        SDL3::SDL3
        sdl3webgpu
        webgpu
)

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()
