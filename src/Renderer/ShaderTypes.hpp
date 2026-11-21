#pragma once
#ifndef BONSAI_SHADER_TYPES_HPP
#define BONSAI_SHADER_TYPES_HPP

#include <Math/Math.hpp>

namespace Bonsai::Shader
{
    /// @brief The Camera is a shader type that contains camera data.
    struct Camera
    {
        alignas(16) glm::mat4 viewProjectMatrix;
    };

    /// @brief The ObjectTransform is a shader type that contains object transform data.
    struct ObjectTransform
    {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 normal;
    };
} // namespace Bonsai::Shader

#endif //BONSAI_OBJECT_TRANSFROM_HPP
