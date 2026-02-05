project("Bonsai Viewer" VERSION 0.1.0)

find_package(cereal REQUIRED)
find_package(glad REQUIRED)
find_package(glm REQUIRED)
find_package(SDL3 REQUIRED)

add_executable(bonsai_viewer
        src/assets/mesh.cpp
        src/assets/mesh.hpp
        src/assets/shader.cpp
        src/assets/shader.hpp
        src/rendering/render_mesh.hpp
        src/rendering/shader_pipeline.hpp
        src/main.cpp
)
target_compile_features(bonsai_viewer PRIVATE cxx_std_17)
target_link_libraries(bonsai_viewer PRIVATE cereal::cereal glad::glad glm::glm SDL3::SDL3)
target_extended_warnings(bonsai_viewer)
target_track_runtime_dll_dependencies(bonsai_viewer)

add_custom_target(bonsai_viewer_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:bonsai_viewer>/assets
)
add_dependencies(bonsai_viewer bonsai_viewer_assets)
