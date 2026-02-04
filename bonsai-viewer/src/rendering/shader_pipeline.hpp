#pragma once
#ifndef BONSAI_VIEWER_RENDERING_SHADER_PIPELINE_HPP
#define BONSAI_VIEWER_RENDERING_SHADER_PIPELINE_HPP

#include <glad/glad.h>

class Shader
{
public:
    GLuint shader = 0;
};

class ShaderPipeline
{
public:
    GLuint program = 0;
};

#endif //BONSAI_VIEWER_RENDERING_SHADER_PIPELINE_HPP
