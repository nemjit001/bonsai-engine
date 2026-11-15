cmake_minimum_required(VERSION 3.28)
project("Bonsai Engine" VERSION 0.1.0)
enable_testing()

# Project options
option(BONSAI_BUILD_TESTS "Build unit tests for Bonsai" ON)

# Enable global options
set(CMAKE_VERIFY_INTERFACE_HEADER_SETS ON)

# Include cmake helpers
include(cmake/dependencies.cmake)
include(cmake/helpers.cmake)

# Find dependencies
find_package(Cereal CONFIG REQUIRED)
find_package(cgltf CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(tinyobjloader CONFIG REQUIRED)
find_package(volk CONFIG REQUIRED)

# Set up bonsai library target
add_library(BonsaiEngineLib STATIC)
add_library(BonsaiEngine::Lib ALIAS BonsaiEngineLib)
target_extended_warnings(BonsaiEngineLib)
target_sources(BonsaiEngineLib
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS src
    FILES
        src/Core/Filesystem.hpp
        src/Core/Logger.hpp
        src/Core/PlatformDefines.hpp
        src/Gfx/GraphicsCommandBuffer.hpp
        src/Gfx/GraphicsDevice.hpp
        src/Gfx/GraphicsTypes.hpp
        src/Math/Math.hpp
        src/Assets/AssetImporter.hpp
        src/Assets/AssetManager.hpp
        src/Assets/AssetType.hpp
        src/Assets/Mesh.hpp
        src/Camera/Camera.hpp
        src/Renderer/CameraConstantBuffer.hpp
        src/Renderer/RenderManager.hpp
        src/Components/CameraComponent.hpp
        src/Components/MeshComponent.hpp
        src/Components/ParentComponent.hpp
        src/Components/RenderMeshComponent.hpp
        src/Components/TransformComponent.hpp
        src/Systems/CameraUploadSystem.hpp
        src/Systems/ECSSystem.hpp
        src/Systems/MeshUploadSystem.hpp
        src/Systems/SystemRegistry.hpp
        src/Scene/Scene.hpp
        src/Scene/SceneManager.hpp
        src/Application.hpp
    PRIVATE
        src/Core/Filesystem.cpp
        src/Core/Logger.cpp
        src/Gfx/GraphicsDevice.cpp
        src/Gfx/Internal/GraphicsCommandBufferInternal.cpp
        src/Gfx/Internal/GraphicsCommandBufferInternal.hpp
        src/Gfx/Internal/GraphicsCommands.hpp
        src/Gfx/Internal/GraphicsTypesInternal.cpp
        src/Gfx/Internal/GraphicsTypesInternal.hpp
        src/Gfx/Vulkan/VulkanCommandRecorder.cpp
        src/Gfx/Vulkan/VulkanCommandRecorder.hpp
        src/Gfx/Vulkan/VulkanGraphicsHelpers.cpp
        src/Gfx/Vulkan/VulkanGraphicsHelpers.hpp
        src/Gfx/Vulkan/VulkanGraphicsTypes.cpp
        src/Gfx/Vulkan/VulkanGraphicsTypes.hpp
        src/Gfx/Vulkan/VulkanObjectFactory.cpp
        src/Gfx/Vulkan/VulkanObjectFactory.hpp
        src/Gfx/Vulkan/VulkanRenderContext.cpp
        src/Gfx/Vulkan/VulkanRenderContext.hpp
        src/Gfx/Vulkan/VulkanTransferContext.cpp
        src/Gfx/Vulkan/VulkanTransferContext.hpp
        src/Gfx/Vulkan/VulkanSwapchain.cpp
        src/Gfx/Vulkan/VulkanSwapchain.hpp
        src/Assets/AssetImporter.cpp
        src/Assets/AssetManager.cpp
        src/Assets/Mesh.cpp
        src/Camera/Camera.cpp
        src/Renderer/RenderManager.cpp
        src/Components/CameraComponent.cpp
        src/Components/MeshComponent.cpp
        src/Components/ParentComponent.cpp
        src/Components/RenderMeshComponent.cpp
        src/Components/TransformComponent.cpp
        src/Systems/CameraUploadSystem.cpp
        src/Systems/MeshUploadSystem.cpp
        src/Systems/SystemRegistry.cpp
        src/Scene/Scene.cpp
        src/Scene/SceneManager.cpp
        src/Application.cpp
)
target_compile_features(BonsaiEngineLib
    PRIVATE
        cxx_std_17
)
target_link_libraries(BonsaiEngineLib
    PUBLIC
        cereal::cereal
        cgltf::cgltf
        glm::glm
        EnTT::EnTT
        SDL3::SDL3
        spdlog::spdlog
        tinyobjloader
        volk::volk_headers
)

# Set up bonsai engine binary target
add_executable(BonsaiEngine WIN32)
target_copy_runtime_dlls(BonsaiEngine)
target_extended_warnings(BonsaiEngine)
target_sources(BonsaiEngine
    PRIVATE
        src/main.cpp
)
target_compile_features(BonsaiEngine
    PRIVATE
        cxx_std_17
)
target_link_libraries(BonsaiEngine
    PRIVATE
        BonsaiEngineLib
)

if (BONSAI_BUILD_TESTS)
    add_subdirectory(tests)
endif()
