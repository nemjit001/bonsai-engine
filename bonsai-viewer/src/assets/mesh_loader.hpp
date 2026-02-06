#pragma once
#ifndef BONSAI_VIEWER_ASSETS_MESH_LOADER_HPP
#define BONSAI_VIEWER_ASSETS_MESH_LOADER_HPP

#include "assets/asset_loader.hpp"
#include "assets/mesh.hpp"

namespace bonsai::assets
{
    class IMeshLoader
    {
    public:
        virtual ~IMeshLoader() = default;

        virtual Mesh load(core::Path const& path) = 0;
    };

    class WavefrontMeshLoader : IMeshLoader
    {
    public:
        static constexpr char const* FILE_EXTENSION = ".obj";

        Mesh load(core::Path const& path) override;
    };
} // namespace bonsai::assets


#endif //BONSAI_VIEWER_ASSETS_MESH_LOADER_HPP
