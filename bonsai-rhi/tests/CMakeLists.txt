project("BonsaiRHITests")
enable_testing()

find_package(GTest CONFIG REQUIRED)

add_executable(BonsaiRHITests)
target_sources(BonsaiRHITests
    PRIVATE
        src/BonsaiRHITests.cpp
        src/RHIShaderCompilerTests.cpp
)
target_compile_features(BonsaiRHITests PRIVATE cxx_std_17)
target_enable_extended_warnings(BonsaiRHITests)
target_link_libraries(BonsaiRHITests
    PRIVATE
        Bonsai::RHI
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(BonsaiRHITests)
