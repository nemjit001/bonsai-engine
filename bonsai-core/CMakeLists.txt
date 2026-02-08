project("Bonsai Core" VERSION 0.1.0)

find_package(cereal REQUIRED)
find_package(EnTT REQUIRED)
find_package(glm REQUIRED)
find_package(tinyobjloader REQUIRED)

add_library(bonsai_core STATIC
        include/bonsai/core/filesystem.hpp
        include/bonsai/assets/asset_loader.hpp
        include/bonsai/assets/mesh.hpp
        include/bonsai/assets/shader.hpp
        src/core/filesystem.cpp
        src/assets/mesh.cpp
        src/assets/shader.cpp
)
target_compile_features(bonsai_core PUBLIC cxx_std_17)
target_include_directories(bonsai_core PUBLIC include PRIVATE src)
target_link_libraries(bonsai_core PUBLIC cereal::cereal EnTT::EnTT glm::glm PRIVATE tinyobjloader::tinyobjloader)
target_extended_warnings(bonsai_core)

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()
