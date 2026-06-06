project("BonsaiEngine"
    VERSION 0.0.1
    SPDX_LICENSE MIT
    LANGUAGES CXX
)

find_package(SDL3 CONFIG REQUIRED)

add_library(BonsaiEngineLib)
add_library(Bonsai::Engine::Lib ALIAS BonsaiEngineLib)
target_sources(BonsaiEngineLib
    PRIVATE
        src/BonsaiBase.hpp
        src/BonsaiEngine.cpp
        src/BonsaiEngine.hpp
        src/Base/PlatformDefines.hpp
        src/Base/PlatformTypes.hpp
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS src/
    FILES
        src/BonsaiBase.hpp
        src/BonsaiEngine.hpp
)
target_compile_features(BonsaiEngineLib PRIVATE cxx_std_17)
target_enable_extended_warnings(BonsaiEngineLib)
target_link_libraries(BonsaiEngineLib
    PRIVATE
        SDL3::SDL3
)

add_executable(BonsaiEngine)
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

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()
