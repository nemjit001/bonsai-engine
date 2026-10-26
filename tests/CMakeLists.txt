project("Bonsai Engine Test")
enable_testing()

# Find dependencies
find_package(GTest CONFIG REQUIRED)

# Set up test target
add_executable(BonsaiEngineTests)
target_sources(BonsaiEngineTests
    PRIVATE
        src/SanityTests.cpp
        src/Assets/AssetManagerTests.cpp
        src/Assets/MeshTests.cpp
        src/Camera/CameraTests.cpp
        src/Scene/SystemRegistryTests.cpp
)
target_compile_features(BonsaiEngineTests
    PRIVATE
        cxx_std_17
)
target_link_libraries(BonsaiEngineTests
    PRIVATE
        GTest::gtest
        GTest::gmock
        GTest::gtest_main
        BonsaiEngine::Lib
)
target_copy_resource_files(BonsaiEngineTests
    resources/assets/cube.obj
)

# Discover tests
include(GoogleTest)
gtest_discover_tests(BonsaiEngineTests)
