#pragma once
#ifndef BONSAI_SHADER_TYPES_HPP
#define BONSAI_SHADER_TYPES_HPP

#include <cstdint>
#include <Math/Math.hpp>

namespace Bonsai::Shader
{
    /// @brief The Camera is a shader type that contains camera data.
    struct Camera
    {
        alignas(16) glm::mat4 viewProjectMatrix;
    };

    /// @brief The RenderObject is a shader type that contains render object data.
    struct RenderObject
    {
        alignas(4) uint32_t objectId;
    };

    /// @brief The ObjectTransform is a shader type that contains object transform data.
    struct ObjectTransform
    {
        alignas(16) glm::mat4 modelMatrix;
        alignas(16) glm::mat4 normalMatrix;
    };
} // namespace Bonsai::Shader

#endif //BONSAI_OBJECT_TRANSFROM_HPP
