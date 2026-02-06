#include "mesh_loader.hpp"

#define TINY_OBJ_LOADER_IMPLEMENTATION
#include <vector>
#include <tiny_obj_loader.h>

namespace bonsai::assets
{
    template<>
    Mesh AssetLoader<Mesh>::load(core::Path const& path)
    {
        std::string const& ext = path.extension().string();
        if (ext == WavefrontMeshLoader::FILE_EXTENSION)
        {
            return WavefrontMeshLoader().load(path);
        }

        return {};
    }

    Mesh WavefrontMeshLoader::load(core::Path const& path)
    {
        tinyobj::ObjReader reader{};
        tinyobj::ObjReaderConfig config{};
        config.triangulate = true;
        config.triangulation_method = "earcut";
        config.vertex_color = true;

        if (reader.ParseFromFile(path.string(), config))
        {
            return {};
        }

        auto const& attrib = reader.GetAttrib();
        auto const& shapes = reader.GetShapes();
        auto const& materials = reader.GetMaterials();
        for (auto const& shape : shapes)
        {
            //
        }

        return {};
    }
} // namespace bonsai::assets

