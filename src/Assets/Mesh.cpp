#include "Mesh.hpp"

#define CGLTF_IMPLEMENTATION
#include <cgltf.h>
#include "AssetImporter.hpp"

namespace Bonsai
{
    template<>
    std::shared_ptr<Mesh> AssetImporter<Mesh>::Import(std::filesystem::path const& path)
    {
        // Convert path to string
        std::string const pathString = path.string();

        // Parse file
        cgltf_options options{};
        cgltf_data* data = nullptr;
        if (cgltf_parse_file(&options, pathString.c_str(), &data) != cgltf_result_success) {
            return {};
        }

        // Load gltf scene as single mesh
        cgltf_scene* scene = data->scene;
        if (scene == nullptr) {
            return {};
        }

        // TODO(nemjit001): Load cgltf scene tree & read in mesh data
        // Do cleanup
        cgltf_free(data);
        return std::make_shared<Mesh>();
    }
} // namespace Bonsai
