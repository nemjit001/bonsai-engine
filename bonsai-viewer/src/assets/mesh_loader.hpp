#pragma once
#ifndef BONSAI_VIEWER_ASSETS_MESH_LOADER_HPP
#define BONSAI_VIEWER_ASSETS_MESH_LOADER_HPP

#include "assets/asset_loader.hpp"
#include "assets/mesh.hpp"

namespace bonsai::assets
{
    /// @brief Mesh loader interface for mesh loaders to implement.
    class IMeshLoader
    {
    public:
        virtual ~IMeshLoader() = default;

        /// @brief Load a mesh stored in a DCC file format from disk.
        /// @param path 
        /// @return The loaded mesh or an empty object.
        virtual std::optional<Mesh> load(core::Path const& path) = 0;
    };

    /// @brief Mesh loader for Wavefront mesh files.
    class WavefrontMeshLoader : IMeshLoader
    {
    public:
        static constexpr char const* FILE_EXTENSION = ".obj";

        std::optional<Mesh> load(core::Path const& path) override;
    };
} // namespace bonsai::assets


#endif //BONSAI_VIEWER_ASSETS_MESH_LOADER_HPP
