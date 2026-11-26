cmake_minimum_required(VERSION 3.28)
project("Bonsai Engine" VERSION 0.1.0)
enable_testing()

# Project options
option(BONSAI_BUILD_TESTS "Build unit tests for Bonsai" ON)
option(BONSAI_BUILD_ENGINE "Build the Bonsai Engine library and binary targets" ON)

# Include cmake helpers
include(cmake/dependencies.cmake)
include(cmake/helpers.cmake)

# Find dependencies
find_package(cereal CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(slang CONFIG REQUIRED)
find_package(tinyobjloader CONFIG REQUIRED)
find_package(volk CONFIG REQUIRED)

# Set up Bonsai Core library target
add_library(BonsaiCore STATIC)
add_library(Bonsai::Core ALIAS BonsaiCore)
target_extended_warnings(BonsaiCore)
target_sources(BonsaiCore
    PUBLIC FILE_SET HEADERS
    BASE_DIRS src
    FILES
        src/Core/Filesystem.hpp
        src/Core/Logger.hpp
        src/Core/PlatformDefines.hpp
    PRIVATE
        src/Core/Filesystem.cpp
        src/Core/Logger.cpp
)
target_compile_features(BonsaiCore
    PRIVATE
        cxx_std_17
)
target_link_libraries(BonsaiCore
    PUBLIC
        spdlog::spdlog
)

# Set up Bonsai Gfx library target
add_library(BonsaiGfx STATIC)
add_library(Bonsai::Gfx ALIAS BonsaiGfx)
target_extended_warnings(BonsaiGfx)
target_sources(BonsaiGfx
    PUBLIC FILE_SET HEADERS
    BASE_DIRS src
    FILES
        src/Gfx/GraphicsCommandBuffer.hpp
        src/Gfx/GraphicsDevice.hpp
        src/Gfx/GraphicsTypes.hpp
        src/Gfx/ShaderCompiler.hpp
    PRIVATE
        src/Gfx/GraphicsDevice.cpp
        src/Gfx/ShaderCompiler.cpp
        src/Gfx/Internal/GraphicsCommandBufferImpl.cpp
        src/Gfx/Internal/GraphicsCommandBufferImpl.hpp
        src/Gfx/Internal/GraphicsCommandBufferPool.cpp
        src/Gfx/Internal/GraphicsCommandBufferPool.hpp
        src/Gfx/Internal/GraphicsCommands.hpp
        src/Gfx/Internal/GraphicsDeviceImpl.cpp
        src/Gfx/Internal/GraphicsDeviceImpl.hpp
        src/Gfx/Internal/GraphicsTypesInternal.cpp
        src/Gfx/Internal/GraphicsTypesInternal.hpp
        src/Gfx/Vulkan/VulkanCommandRecorder.cpp
        src/Gfx/Vulkan/VulkanCommandRecorder.hpp
        src/Gfx/Vulkan/VulkanDescriptorSetAllocator.cpp
        src/Gfx/Vulkan/VulkanDescriptorSetAllocator.hpp
        src/Gfx/Vulkan/VulkanGraphicsDevice.cpp
        src/Gfx/Vulkan/VulkanGraphicsDevice.hpp
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
)
target_compile_features(BonsaiGfx
    PRIVATE
        cxx_std_17
)
target_link_libraries(BonsaiGfx
    PRIVATE
        volk::volk_headers
    PUBLIC
        Bonsai::Core
        SDL3::SDL3
        slang::slang
)

# Set up Bonsai Math lib
add_library(BonsaiMath STATIC)
add_library(Bonsai::Math ALIAS BonsaiMath)
target_extended_warnings(BonsaiMath)
target_sources(BonsaiMath
    PUBLIC FILE_SET HEADERS
    BASE_DIRS src
    FILES
        src/Math/Math.hpp
    PRIVATE
        src/Math/Math.cpp
)
target_compile_features(BonsaiMath
    PRIVATE
        cxx_std_17
)
target_link_libraries(BonsaiMath
    PUBLIC
        glm::glm
)

# Set up bonsai library target
add_library(BonsaiEngineLib STATIC)
add_library(BonsaiEngine::Lib ALIAS BonsaiEngineLib)
target_extended_warnings(BonsaiEngineLib)
target_sources(BonsaiEngineLib
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS src
    FILES
        src/Assets/AssetHandle.hpp
        src/Assets/AssetHandle.inl
        src/Assets/AssetImporter.hpp
        src/Assets/AssetManager.hpp
        src/Assets/AssetManager.inl
        src/Assets/AssetType.hpp
        src/Assets/Material.hpp
        src/Assets/Mesh.hpp
        src/Camera/Camera.hpp
        src/Renderer/RenderManager.hpp
        src/Renderer/RenderObjectCache.hpp
        src/Renderer/RenderWorld.hpp
        src/Renderer/ShaderTypes.hpp
        src/Renderer/Subrenderers/GeometryRenderer.hpp
        src/Renderer/Subrenderers/PresentRenderer.hpp
        src/Renderer/Subrenderers/Subrenderer.hpp
        src/Components/CameraComponent.hpp
        src/Components/MeshComponent.hpp
        src/Components/ParentComponent.hpp
        src/Components/TransformComponent.hpp
        src/Systems/ECSSystem.hpp
        src/Systems/SystemRegistry.hpp
        src/Systems/Internal/CameraInitSystem.hpp
        src/Systems/Internal/GeometryRenderSystem.hpp
        src/Systems/Internal/SyncRenderWorldSystem.hpp
        src/Systems/Internal/UploadRenderWorldSystem.hpp
        src/Scene/Scene.hpp
        src/Scene/SceneManager.hpp
        src/Application/FileDropHandler.hpp
        src/Application.hpp
    PRIVATE
        src/Assets/AssetImporter.cpp
        src/Assets/Material.cpp
        src/Assets/Mesh.cpp
        src/Camera/Camera.cpp
        src/Renderer/RenderManager.cpp
        src/Renderer/RenderObjectCache.cpp
        src/Renderer/RenderWorld.cpp
        src/Renderer/Subrenderers/GeometryRenderer.cpp
        src/Renderer/Subrenderers/PresentRenderer.cpp
        src/Components/CameraComponent.cpp
        src/Components/MeshComponent.cpp
        src/Components/ParentComponent.cpp
        src/Components/TransformComponent.cpp
        src/Systems/SystemRegistry.cpp
        src/Systems/Internal/CameraInitSystem.cpp
        src/Systems/Internal/GeometryRenderSystem.cpp
        src/Systems/Internal/SyncRenderWorldSystem.cpp
        src/Systems/Internal/UploadRenderWorldSystem.cpp
        src/Scene/Scene.cpp
        src/Scene/SceneManager.cpp
        src/Application/FileDropHandler.cpp
        src/Application.cpp
)
target_compile_features(BonsaiEngineLib
    PRIVATE
        cxx_std_17
)
target_link_libraries(BonsaiEngineLib
    PRIVATE
        tinyobjloader::tinyobjloader
    PUBLIC
        Bonsai::Core
        Bonsai::Gfx
        Bonsai::Math
        cereal::cereal
        EnTT::EnTT
        SDL3::SDL3
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
        BonsaiEngine::Lib
)
target_copy_resource_files(BonsaiEngine
    resources/shaders/common.slang
    resources/shaders/fullscreen_quad.slang
    resources/shaders/unlit.slang
)

if (BONSAI_BUILD_TESTS)
    add_subdirectory(tests)
endif()
