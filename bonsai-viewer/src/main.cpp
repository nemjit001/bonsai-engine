#include <cstdio>
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "core/filesystem.hpp"

int main(int, char*[])
{
    // Check executable location
    bonsai::core::Path const program_dir = bonsai::core::FileSystem::program_dir();
    printf("Running from \"%s\"\n", program_dir.string().c_str());

    // Init ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& imgui_io = ImGui::GetIO();
    imgui_io.IniFilename = nullptr;
    imgui_io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_IsSRGB;

    // Init SDL
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        return 1;
    }

    // Init OpenGL
    SDL_Window* window = SDL_CreateWindow("Bonsai Viewer", 1280, 720, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_OPENGL);
    if (!window)
    {
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, true);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (!gl_context)
    {
        return 1;
    }

    SDL_GL_MakeCurrent(window, gl_context);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress)))
    {
        return 1;
    }

    // Init ImGui backends
    if (!ImGui_ImplSDL3_InitForOpenGL(window, gl_context) || !ImGui_ImplOpenGL3_Init("#version 450"))
    {
        return 1;
    }

    SDL_GL_SetSwapInterval(0);
    glEnable(GL_FRAMEBUFFER_SRGB);

    bool running = true;
    while (running)
    {
        // Process SDL event
        SDL_Event event{};
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }

            ImGui_ImplSDL3_ProcessEvent(&event);
        }

        // Draw GUI
        ImGui_ImplSDL3_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();
        ImGui::EndFrame();

        // Render frame
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.1F, 0.1F, 0.1F, 1.0F);
        glClearDepth(1.0F);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glFinish();
        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();

    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);

    SDL_Quit();

    ImGui::DestroyContext();

    return 0;
}
