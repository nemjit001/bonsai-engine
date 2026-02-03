project("Bonsai Viewer" VERSION 0.1.0)

add_executable(bonsai_viewer
        src/main.cpp
)
target_compile_features(bonsai_viewer PRIVATE cxx_std_17)
target_link_libraries(bonsai_viewer PRIVATE)
target_extended_warnings(bonsai_viewer)
target_track_runtime_dll_dependencies(bonsai_viewer)
