project("BonsaiEngine"
    VERSION 0.0.1
    LANGUAGES CXX
)

find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3WebGPU CONFIG REQUIRED)
find_package(WebGPU CONFIG REQUIRED)

configure_file(BonsaiEngineConfig.hpp.in BonsaiEngineConfig.hpp)

add_library(BonsaiEngineLib)
add_library(Bonsai::Engine::Lib ALIAS BonsaiEngineLib)
target_sources(BonsaiEngineLib
    PRIVATE
        src/BonsaiBase.hpp
        src/BonsaiGraphics.hpp
        src/BonsaiCore.hpp
        src/BonsaiEngine.cpp
        src/BonsaiEngine.hpp
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
        src/Core/CLIArgParser.cpp
        src/Core/CLIArgParser.hpp
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS src/
    FILES
        src/BonsaiBase.hpp
        src/Containers/DynamicArray.hpp
        src/Containers/HashMap.hpp
        src/Containers/StaticArray.hpp
        src/BonsaiGraphics.hpp
        src/BonsaiCore.hpp
        src/BonsaiEngine.hpp
)
target_include_directories(BonsaiEngineLib PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_compile_features(BonsaiEngineLib PRIVATE cxx_std_17)
target_enable_extended_warnings(BonsaiEngineLib)
target_link_libraries(BonsaiEngineLib
    PRIVATE
        SDL3::SDL3
        sdl3webgpu
        webgpu
)

add_executable(BonsaiEngine WIN32)
target_sources(BonsaiEngine
    PRIVATE
        src/main.cpp
)
target_compile_features(BonsaiEngine PRIVATE cxx_std_17)
target_enable_extended_warnings(BonsaiEngine)
target_link_libraries(BonsaiEngine
    PRIVATE
        BonsaiEngineLib
)
target_copy_dll_dependencies(BonsaiEngine
    SDL3::SDL3
    webgpu
)

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()
