project("Bonsai Viewer Tests")

find_package(GTest CONFIG REQUIRED)

add_executable(BonsaiViewerTests
        SceneTests.cpp
        RenderBackendTests.cpp
)
target_link_libraries(BonsaiViewerTests PRIVATE BonsaiViewerLib GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(BonsaiViewerTests)
