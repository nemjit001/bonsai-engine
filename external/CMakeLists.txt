message(STATUS "Setting up external dependencies")

# Set up ImGui
find_package(SDL3 REQUIRED)
add_library(imgui STATIC
        imgui/imgui.cpp
        imgui/imgui_demo.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/backends/imgui_impl_sdl3.cpp
)
target_include_directories(imgui PUBLIC imgui imgui/backends)
target_link_libraries(imgui PUBLIC SDL3::SDL3)
add_library(imgui::imgui ALIAS imgui)

if (BONSAI_VULKAN_SUPPORT)
    find_package(volk REQUIRED)
    target_sources(imgui PUBLIC imgui/backends/imgui_impl_vulkan.cpp)
    target_link_libraries(imgui PUBLIC volk::volk_headers)
    target_compile_definitions(imgui PUBLIC IMGUI_IMPL_VULKAN_USE_VOLK)
endif()

if (BONSAI_DX12_SUPPORT)
    target_sources(imgui PUBLIC imgui/backends/imgui_impl_dx12.cpp)
    target_link_libraries(imgui PUBLIC dxgi d3d12)
endif()
