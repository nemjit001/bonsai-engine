#pragma once
#ifndef BONSAI_VIEWER_RENDERING_RENDER_MESH_HPP
#define BONSAI_VIEWER_RENDERING_RENDER_MESH_HPP

#include <glad/glad.h>

class RenderMesh
{
public:
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;
};

#endif //BONSAI_VIEWER_RENDERING_RENDER_MESH_HPP
