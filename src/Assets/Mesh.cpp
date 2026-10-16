#include "Mesh.hpp"

#include "AssetImporter.hpp"

namespace Bonsai
{
    template<>
    std::shared_ptr<Mesh> AssetImporter<Mesh>::Import(std::filesystem::path const& path)
    {
        return std::make_shared<Mesh>();
    }
} // namespace Bonsai
