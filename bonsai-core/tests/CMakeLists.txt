project("Bonsai Core Tests")

find_package(GTest REQUIRED)

add_executable(bonsai_core_tests
        src/sanity.cpp
        src/core/filesystem_tests.cpp
        src/subsystems/logging_manager_tests.cpp
)
target_compile_features(bonsai_core_tests PRIVATE cxx_std_17)
target_include_directories(bonsai_core_tests PRIVATE . ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(bonsai_core_tests PRIVATE GTest::gtest GTest::gtest_main bonsai_core)

configure_file(test_config.hpp.in test_config.hpp)

include(GoogleTest)
gtest_discover_tests(bonsai_core_tests)
