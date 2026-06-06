project("BonsaiEngine"
    VERSION 0.0.1
    SPDX_LICENSE MIT
    LANGUAGES CXX
)

find_package(SDL3 CONFIG REQUIRED)

add_executable(BonsaiEngine)
target_sources(BonsaiEngine
    PRIVATE
        src/main.cpp
)
target_compile_features(BonsaiEngine PRIVATE cxx_std_17)
target_enable_extended_warnings(BonsaiEngine)
target_link_libraries(BonsaiEngine
    PRIVATE
        SDL3::SDL3
)

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()
