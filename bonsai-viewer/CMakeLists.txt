project("Bonsai Viewer" VERSION 0.1.0)

find_package(SDL3 REQUIRED)
find_package(volk REQUIRED)

add_library(BonsaiViewerLib
        src/Engine.cpp
        src/Engine.hpp
        src/Logging.cpp
        src/Logging.hpp
        src/Renderer.cpp
        src/Renderer.hpp
        src/Scene.cpp
        src/Scene.hpp
        src/Scene/RenderObject.cpp
        src/Scene/RenderObject.hpp
        src/Scene/SceneObject.cpp
        src/Scene/SceneObject.hpp
        src/Scene/Transform.cpp
        src/Scene/Transform.hpp
)
target_compile_features(BonsaiViewerLib PUBLIC cxx_std_17)
target_include_directories(BonsaiViewerLib PUBLIC src)
target_link_libraries(BonsaiViewerLib PUBLIC SDL3::SDL3 volk::volk_headers)
target_extended_warnings(BonsaiViewerLib)

add_executable(BonsaiViewer
        src/main.cpp
)
target_compile_features(BonsaiViewer PRIVATE cxx_std_17)
target_include_directories(BonsaiViewer PRIVATE src)
target_link_libraries(BonsaiViewer PRIVATE BonsaiViewerLib)
target_extended_warnings(BonsaiViewer)

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
