project("Bonsai Viewer" VERSION 0.1.0)

find_package(SDL3 REQUIRED)
find_package(volk REQUIRED)

add_executable(bonsai_viewer
        src/main.cpp
        src/Engine.cpp
        src/Engine.hpp
        src/Renderer.cpp
        src/Renderer.hpp
)
target_compile_features(bonsai_viewer PRIVATE cxx_std_17)
target_include_directories(bonsai_viewer PRIVATE src)
target_link_libraries(bonsai_viewer PRIVATE SDL3::SDL3 volk::volk_headers)
target_extended_warnings(bonsai_viewer)
