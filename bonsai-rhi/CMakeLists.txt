project("BonsaiRHI"
    VERSION 0.0.1
    DESCRIPTION "Render Hardware Interface for the Bonsai Engine."
    LANGUAGES CXX
)

# Set up shared dependencies
find_package(SDL3 CONFIG REQUIRED)

# Create library target
add_library(BonsaiRHI SHARED)
add_library(Bonsai::RHI ALIAS BonsaiRHI)

# Configure library
target_sources(BonsaiRHI
    PRIVATE
        src/BonsaiRHI.cpp
        src/BonsaiRHITypes.hpp
        src/RHIShaderCompiler.cpp
        src/ShaderCompiler/SlangShaderCompiler.cpp
        src/ShaderCompiler/SlangShaderCompiler.hpp
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS src/
    FILES
        src/BonsaiRHI.hpp
        src/BonsaiRHIExport.hpp
        src/RHIShaderCompiler.hpp
)
target_compile_features(BonsaiRHI PRIVATE cxx_std_17)
target_enable_extended_warnings(BonsaiRHI)
target_compile_definitions(BonsaiRHI
    PRIVATE
        BONSAI_RHI_EXPORT_SYMBOLS=1
)
target_link_libraries(BonsaiRHI
    PUBLIC
        Bonsai::Base
        SDL3::SDL3
)

# Configure supported backend APIs
set(BONSAI_RHI_VULKAN_SUPPORT OFF)
set(BONSAI_RHI_D3D12_SUPPORT OFF)
set(BONSAI_RHI_WEBGPU_SUPPORT OFF)
if (WIN32)
    set(BONSAI_RHI_TEST_VULKAN_SUPPORT ON)
    set(BONSAI_RHI_TEST_D3D12_SUPPORT ON)
    set(BONSAI_RHI_TEST_WEBGPU_SUPPORT ON)
elseif(LINUX)
    set(BONSAI_RHI_TEST_VULKAN_SUPPORT ON)
    set(BONSAI_RHI_TEST_WEBGPU_SUPPORT ON)
endif()

if (BONSAI_RHI_TEST_VULKAN_SUPPORT)
    message(STATUS "BonsaiRHI: Testing Vulkan support")
    find_package(Volk CONFIG)

    if (Volk_FOUND)
        message(STATUS "BonsaiRHI: Building with Vulkan support")
        set(BONSAI_RHI_VULKAN_SUPPORT ON)

        target_link_libraries(BonsaiRHI
            PRIVATE
                volk::volk_headers
        )
    endif()
endif()

if (BONSAI_RHI_TEST_D3D12_SUPPORT)
    message(STATUS "BonsaiRHI: Testing D3D12 support")
    if (WIN32)
        message(STATUS "BonsaiRHI: Building with D3D12 support")
        set(BONSAI_RHI_D3D12_SUPPORT ON)

        target_link_libraries(BonsaiRHI
            PRIVATE
                d3d12
                dxgi
        )
    endif()
endif()

if (BONSAI_RHI_TEST_WEBGPU_SUPPORT)
    message(STATUS "BonsaiRHI: Testing WebGPU support")
    find_package(WebGPU CONFIG)
    find_package(sdl3webgpu CONFIG)

    if (WebGPU_FOUND AND sdl3webgpu_FOUND)
        message(STATUS "BonsaiRHI: Building with WebGPU support")
        set(BONSAI_RHI_WEBGPU_SUPPORT ON)

        target_link_libraries(BonsaiRHI
            PRIVATE
                webgpu
                sdl3webgpu
        )
    endif()
endif()

message(STATUS "BonsaiRHI: Found API support:")
message(STATUS "    BonsaiRHI Vulkan support:   ${BONSAI_RHI_VULKAN_SUPPORT}")
message(STATUS "    BonsaiRHI D3D12 support:    ${BONSAI_RHI_D3D12_SUPPORT}")
message(STATUS "    BonsaiRHI WebGPU support:   ${BONSAI_RHI_WEBGPU_SUPPORT}")

target_compile_definitions(BonsaiRHI
    PRIVATE
        BONSAI_RHI_VULKAN_SUPPORT=$<BOOL:${BONSAI_RHI_VULKAN_SUPPORT}>
        BONSAI_RHI_D3D12_SUPPORT=$<BOOL:${BONSAI_RHI_D3D12_SUPPORT}>
        BONSAI_RHI_WEBGPU_SUPPORT=$<BOOL:${BONSAI_RHI_WEBGPU_SUPPORT}>
)

# Configure unit tests
if (BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Add install target
install(
    TARGETS BonsaiRHI
    FILE_SET HEADERS
    FILE_SET generated_headers
)
