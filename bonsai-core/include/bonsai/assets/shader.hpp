#pragma once
#ifndef BONSAI_CORE_ASSETS_SHADER_HPP
#define BONSAI_CORE_ASSETS_SHADER_HPP

#include <string>
#include <cereal/cereal.hpp>
#include "asset_loader.hpp"

namespace bonsai::assets
{
    /// @brief The shader class is used to store shader assets. Shader assets consist of one or more shader files
    /// that combine into a shader pipeline. Fixed function state for the shader pipeline can also be specified in the shader asset.
    class Shader
    {
    public:
        template<class Archive>
        void serialize(Archive& archive)
        {
            archive(
                CEREAL_NVP(name),
                CEREAL_NVP(vertex_shader),
                CEREAL_NVP(fragment_shader)
            );
        }

    public:
        std::string name;
        std::string vertex_shader;
        std::string fragment_shader;
    };

    template<>
    std::optional<Shader> AssetLoader<Shader>::load(core::Path const& path);
} // namespace bonsai::assets

#endif //BONSAI_CORE_ASSETS_SHADER_HPP
