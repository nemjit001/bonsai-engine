project("Bonsai Core" VERSION 0.1.0)

find_package(cereal REQUIRED)
find_package(EnTT REQUIRED)
find_package(glm REQUIRED)
find_package(SDL3 REQUIRED)
find_package(spdlog REQUIRED)
find_package(tinyobjloader REQUIRED)

add_library(bonsai_core STATIC
        include/bonsai/core/filesystem.hpp
        include/bonsai/core/logging.hpp
        include/bonsai/subsystems/engine_event_manager.hpp
        include/bonsai/subsystems/engine_event.hpp
        include/bonsai/subsystems/engine_subsystem.hpp
        include/bonsai/subsystems/logging_manager.hpp
        include/bonsai/subsystems/main_loop_manager.hpp
        include/bonsai/subsystems/window_manager.hpp
        include/bonsai/assets/asset_loader.hpp
        include/bonsai/assets/mesh.hpp
        include/bonsai/assets/shader.hpp
        src/core/filesystem.cpp
        src/subsystems/engine_event_manager.cpp
        src/subsystems/engine_subsystem.cpp
        src/subsystems/logging_manager.cpp
        src/subsystems/main_loop_manager.cpp
        src/subsystems/window_manager.cpp
        src/assets/mesh.cpp
        src/assets/shader.cpp
)
target_compile_features(bonsai_core PUBLIC cxx_std_17)
target_include_directories(bonsai_core PUBLIC include PRIVATE src)
target_link_libraries(bonsai_core PUBLIC cereal::cereal EnTT::EnTT glm::glm spdlog::spdlog PRIVATE SDL3::SDL3 tinyobjloader::tinyobjloader)
target_extended_warnings(bonsai_core)

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()
