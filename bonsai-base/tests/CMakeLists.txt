project("BonsaiBaseTests")
enable_testing()

find_package(GTest CONFIG REQUIRED)

add_executable(BonsaiBaseTests)
target_sources(BonsaiBaseTests
    PRIVATE
        src/Base/AllocatorTests.cpp
        src/Base/FileSystemTests.cpp
        src/Base/MemoryTests.cpp
)
target_compile_features(BonsaiBaseTests PRIVATE cxx_std_17)
target_enable_extended_warnings(BonsaiBaseTests)
target_link_libraries(BonsaiBaseTests
    PRIVATE
        Bonsai::Base
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(BonsaiBaseTests)
