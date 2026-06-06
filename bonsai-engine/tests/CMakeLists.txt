project("BonsaiEngineTests")
enable_testing()

find_package(GTest CONFIG REQUIRED)

add_executable(BonsaiEngineTests)
target_sources(BonsaiEngineTests
    PRIVATE
        src/SanityTests.cpp
)
target_link_libraries(BonsaiEngineTests PRIVATE
    Bonsai::Engine::Lib
    GTest::gtest
    GTest::gmock
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(BonsaiEngineTests)
