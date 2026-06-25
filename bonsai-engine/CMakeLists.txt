project("BonsaiEngine"
    VERSION 0.0.1
    LANGUAGES CXX
)

find_package(EnTT CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

configure_file(BonsaiEngineConfig.hpp.in BonsaiEngineConfig.hpp)

add_library(BonsaiEngineLib)
add_library(Bonsai::Engine::Lib ALIAS BonsaiEngineLib)
target_sources(BonsaiEngineLib
    PRIVATE
        src/BonsaiEngine.cpp
        src/Core/CLIArgParser.cpp
        src/Core/SystemScheduler.cpp
        src/Core/World.cpp
        src/Math/AABB.cpp
        src/Assets/Level.cpp
        src/Assets/Mesh.cpp
        src/Systems/RenderSystem.cpp
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS src/
    FILES
        src/Assets.hpp
        src/BonsaiEngine.hpp
        src/Core.hpp
        src/Math.hpp
        src/Core/CLIArgParser.hpp
        src/Core/System.hpp
        src/Core/SystemScheduler.hpp
        src/Core/World.hpp
        src/Math/AABB.hpp
        src/Math/MathTypes.hpp
        src/Assets/AssetType.hpp
        src/Assets/Level.hpp
        src/Assets/Mesh.hpp
        src/Components/MeshComponent.hpp
        src/Components/TransformComponent.hpp
        src/Systems/RenderSystem.hpp
)
target_include_directories(BonsaiEngineLib PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_compile_features(BonsaiEngineLib PRIVATE cxx_std_17)
target_enable_extended_warnings(BonsaiEngineLib)
target_link_libraries(BonsaiEngineLib
    PUBLIC
        Bonsai::Base
        EnTT::EnTT
        glm::glm
)

add_executable(BonsaiEngine WIN32)
target_sources(BonsaiEngine
    PRIVATE
        src/main.cpp
)
target_compile_features(BonsaiEngine PRIVATE cxx_std_17)
target_enable_extended_warnings(BonsaiEngine)
target_link_libraries(BonsaiEngine
    PRIVATE
        BonsaiEngineLib
)

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()
