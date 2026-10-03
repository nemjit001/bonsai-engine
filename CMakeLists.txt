cmake_minimum_required(VERSION 3.28)
project("Bonsai Engine" VERSION 0.1.0)

# Include cmake helpers
include(cmake/dependencies.cmake)
include(cmake/helpers.cmake)

# Find dependencies
find_package(Cereal CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

# Set up targets
add_executable(BonsaiEngine)
target_copy_runtime_dlls(BonsaiEngine)
target_extended_warnings(BonsaiEngine)
target_sources(BonsaiEngine
    PRIVATE
    FILE_SET HEADERS
    BASE_DIRS src
    FILES
        src/Core/PlatformDefines.hpp
        src/Core/Logger.hpp
        src/Math/Math.hpp
        src/Scene/ECSSystem.hpp
        src/Scene/Scene.hpp
        src/Scene/SceneManager.hpp
        src/Scene/SystemRegistry.hpp
        src/Components/ParentComponent.hpp
        src/Components/TransformComponent.hpp
        src/Application.hpp
    PRIVATE
        src/Core/Logger.cpp
        src/Scene/Scene.cpp
        src/Scene/SceneManager.cpp
        src/Scene/SystemRegistry.cpp
        src/Components/ParentComponent.cpp
        src/Components/TransformComponent.cpp
        src/Application.cpp
        src/main.cpp
)
target_compile_features(BonsaiEngine
    PRIVATE
        cxx_std_17
)
target_link_libraries(BonsaiEngine
    PRIVATE
        cereal::cereal
        glm::glm
        EnTT::EnTT
        SDL3::SDL3
        spdlog::spdlog
)
