project("Bonsai Viewer" VERSION 0.1.0)

find_package(directx-dxc REQUIRED)
find_package(directx-headers REQUIRED)
find_package(glm REQUIRED)
find_package(SDL3 REQUIRED)
find_package(tinyobjloader REQUIRED)
find_package(unofficial-spirv-reflect REQUIRED)
find_package(volk REQUIRED)
find_package(Vulkan REQUIRED)
find_package(VulkanMemoryAllocator REQUIRED)

set(BONSAI_VULKAN_LIBS
        GPUOpen::VulkanMemoryAllocator
        unofficial::spirv-reflect
        volk::volk_headers
        Vulkan::Headers
)

set(BONSAI_DX12_LIBS
        dxgi
        d3d12
        Microsoft::DirectX-Headers
)

add_library(BonsaiViewerLib
        src/BonsaiMath.cpp
        src/BonsaiMath.hpp
        src/Engine.cpp
        src/Engine.hpp
        src/Logging.cpp
        src/Logging.hpp
        src/Renderer.hpp
        src/Scene.cpp
        src/Scene.hpp
        src/Assets/StaticMesh.cpp
        src/Assets/StaticMesh.hpp
        src/Renderer/RenderHandle.hpp
        src/Renderer/RenderResourcePool.hpp
        src/Renderer/RenderResourcePool.inl
        src/Renderer/ShaderCompiler.cpp
        src/Renderer/ShaderCompiler.hpp
        src/Renderer/VulkanRenderer.cpp
        src/Renderer/VulkanRenderer.hpp
        src/Scene/Camera.cpp
        src/Scene/Camera.hpp
        src/Scene/Light.cpp
        src/Scene/Light.hpp
        src/Scene/RenderableObject.cpp
        src/Scene/RenderableObject.hpp
        src/Scene/SceneObject.cpp
        src/Scene/SceneObject.hpp
        src/Scene/Transform.cpp
        src/Scene/Transform.hpp
)
target_compile_features(BonsaiViewerLib PUBLIC cxx_std_17)
target_include_directories(BonsaiViewerLib PUBLIC src)
target_link_libraries(BonsaiViewerLib PUBLIC
        glm::glm
        imgui::imgui
        Microsoft::DirectXShaderCompiler
        SDL3::SDL3 
        tinyobjloader::tinyobjloader
)
target_extended_warnings(BonsaiViewerLib)

if (BONSAI_VULKAN_SUPPORT)
    target_link_libraries(BonsaiViewerLib PUBLIC ${BONSAI_VULKAN_LIBS})
    target_compile_definitions(BonsaiViewerLib PUBLIC BONSAI_VULKAN_SUPPORT=1)
endif()

if (BONSAI_DX12_SUPPORT)
    target_link_libraries(BonsaiViewerLib PUBLIC ${BONSAI_DX12_LIBS})
    target_compile_definitions(BonsaiViewerLib PUBLIC BONSAI_DX12_SUPPORT=1)
endif()

add_executable(BonsaiViewer
        src/main.cpp
)
target_compile_features(BonsaiViewer PRIVATE cxx_std_17)
target_include_directories(BonsaiViewer PRIVATE src)
target_link_libraries(BonsaiViewer PRIVATE BonsaiViewerLib)
target_extended_warnings(BonsaiViewer)

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
