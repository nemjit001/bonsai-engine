#include "Material.hpp"

namespace Bonsai
{
    Material::Material(glm::vec3 const& albedo, float metallic, float roughness)
        :
        _albedo(albedo),
        _metallic(metallic),
        _roughness(roughness)
    {
        //
    }
} // namespace Bonsai
