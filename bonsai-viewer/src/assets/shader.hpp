#pragma once
#ifndef BONSAI_VIEWER_ASSETS_SHADER_HPP
#define BONSAI_VIEWER_ASSETS_SHADER_HPP

#include <string>
#include <cereal/cereal.hpp>

namespace bonsai::assets
{
    /// @brief The shader class is used to store shader assets. Shader assets consist of one or more shader files
    /// that combine into a shader pipeline. Fixed function state for the shader pipeline can also be specified in the shader asset.
    class Shader
    {
    public:
        template<class Archive>
        void serialize(Archive& archive);

    public:
        std::string vertex_shader;
        std::string fragment_shader;
    };

    template<class Archive>
    void Shader::serialize(Archive& archive)
    {
        archive(
            CEREAL_NVP(vertex_shader),
            CEREAL_NVP(fragment_shader)
        );
    }
} // namespace bonsai::assets

#endif //BONSAI_VIEWER_ASSETS_SHADER_HPP
