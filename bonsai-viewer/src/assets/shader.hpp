#pragma once
#ifndef BONSAI_VIEWER_ASSETS_SHADER_HPP
#define BONSAI_VIEWER_ASSETS_SHADER_HPP

#include <string>
#include <cereal/cereal.hpp>

namespace bonsai::assets
{
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
