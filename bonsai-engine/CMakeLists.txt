project("BonsaiEngine"
    VERSION 0.0.1
    LANGUAGES CXX
)

# Set up dependencies
find_package(EnTT CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)

# Create library target
add_library(BonsaiEngineLib STATIC)
add_library(Bonsai::Engine::Lib ALIAS BonsaiEngineLib)

# Configure library
configure_file(BonsaiEngineConfig.hpp.in BonsaiEngineConfig.hpp)
target_sources(BonsaiEngineLib
    PRIVATE
        src/BonsaiEngine.cpp
        src/Core/CLIArgParser.cpp
        src/Core/EventLoopManager.cpp
        src/Core/RenderDeviceManager.cpp
        src/Math/AABB.cpp
        src/Assets/Level.cpp
        src/Assets/Mesh.cpp
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS src/
    FILES
        src/BonsaiEngine.hpp
        src/Core.hpp
        src/Core/CLIArgParser.hpp
        src/Core/EngineSubsystem.hpp
        src/Core/EventLoopManager.hpp
        src/Core/RenderDeviceManager.hpp
        src/Math.hpp
        src/Math/AABB.hpp
        src/Math/MathTypes.hpp
        src/Assets.hpp
        src/Assets/AssetType.hpp
        src/Assets/Level.hpp
        src/Assets/Mesh.hpp
    PUBLIC
    FILE_SET generated_headers
    TYPE HEADERS
    BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/BonsaiEngineConfig.hpp
)
target_compile_features(BonsaiEngineLib PRIVATE cxx_std_17)
target_enable_extended_warnings(BonsaiEngineLib)
target_link_libraries(BonsaiEngineLib
    PUBLIC
        Bonsai::Base
        Bonsai::RHI
        EnTT::EnTT
        glm::glm
        SDL3::SDL3
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

# Add install target
install(
    TARGETS BonsaiEngineLib
    FILE_SET HEADERS
    FILE_SET generated_headers
)
