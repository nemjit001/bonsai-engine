project("BonsaiEngineTests")
enable_testing()

find_package(GTest CONFIG REQUIRED)

include(GoogleTest)

add_executable(BonsaiEngineTests)
target_sources(BonsaiEngineTests
    PRIVATE
        src/SanityTests.cpp
        src/Base/AllocatorTests.cpp
        src/Base/MemoryTests.cpp
)
target_compile_features(BonsaiEngineTests PRIVATE cxx_std_17)
target_enable_extended_warnings(BonsaiEngineTests)
target_link_libraries(BonsaiEngineTests
    PRIVATE
        Bonsai::Engine::Lib
        GTest::gtest_main
)
target_copy_dll_dependencies(BonsaiEngineTests
    SDL3::SDL3
)

gtest_discover_tests(BonsaiEngineTests)
