project("BonsaiEngineTests")
enable_testing()

find_package(GTest CONFIG REQUIRED)

add_executable(BonsaiEngineTests)
target_sources(BonsaiEngineTests
    PRIVATE
        src/SanityTests.cpp
        src/Core/CLIArgParserTests.cpp
        src/Systems/SystemTests.cpp
)
target_compile_features(BonsaiEngineTests PRIVATE cxx_std_17)
target_enable_extended_warnings(BonsaiEngineTests)
target_link_libraries(BonsaiEngineTests
    PRIVATE
        Bonsai::Engine::Lib
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(BonsaiEngineTests)
