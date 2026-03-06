project("Bonsai Viewer" VERSION 0.1.0)

find_package(SDL3 REQUIRED)
find_package(volk REQUIRED)

add_executable(BonsaiViewer
        src/main.cpp
        src/Logging.cpp
        src/Logging.hpp
        src/Engine.cpp
        src/Engine.hpp
        src/Renderer.cpp
        src/Renderer.hpp
)
target_compile_features(BonsaiViewer PRIVATE cxx_std_17)
target_include_directories(BonsaiViewer PRIVATE src)
target_link_libraries(BonsaiViewer PRIVATE SDL3::SDL3 volk::volk_headers)
target_extended_warnings(BonsaiViewer)
