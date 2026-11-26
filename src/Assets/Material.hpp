#pragma once
#ifndef BONSAI_MATERIAL_HPP
#define BONSAI_MATERIAL_HPP

#include <Math/Math.hpp>
#include "Assets/AssetType.hpp"

namespace Bonsai
{
    /// @brief The Material asset type contains host-side material data for rendering.
    class Material : public IAssetType
    {
    public:
        Material() = default;
        Material(glm::vec3 const& albedo, float metallic, float roughness);

        /// @brief Get the dirty flag for the material. This will be set when host-side data is updated.
        /// @return The dirty flag for the material.
        bool GetDirtyFlag() const { return _isDirty; }

        /// @brief Set the dirty flag for the material.
        /// @param isDirty The new dirty flag value for the material.
        void SetDirtyFlag(bool isDirty) { _isDirty = isDirty; }

        /// @brief Get the material albedo value.
        /// @return The material albedo value.
        glm::vec3 const& GetAlbedo() const { return _albedo; }

        /// @brief Get the material metallic value.
        /// @return The material metallic value.
        float GetMetallic() const { return _metallic; }

        /// @brief Get the material roughness value.
        /// @return The material roughness value.
        float GetRoughness() const { return _roughness; }

    private:
        bool _isDirty = true;
        glm::vec3 _albedo = { 0.5F, 0.5F, 0.5F };
        float _metallic = 0.0F;
        float _roughness = 0.5F;
    };
} // namespace Bonsai

#endif //BONSAI_MATERIAL_HPP
