project("Bonsai Core Tests")

find_package(GTest REQUIRED)

add_executable(bonsai_core_tests
        sanity.cpp
)
target_compile_features(bonsai_core_tests PRIVATE cxx_std_17)
target_link_libraries(bonsai_core_tests PRIVATE GTest::gtest GTest::gtest_main bonsai_core)
target_track_runtime_dll_dependencies(bonsai_core_tests)

include(GoogleTest)
gtest_discover_tests(bonsai_core_tests)
