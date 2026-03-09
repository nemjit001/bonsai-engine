#pragma once
#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "BonsaiMath.hpp"
#include "SceneObject.hpp"

/// @brief Supported light types for Bonsai.
enum LightType : uint32_t
{
    LightType_PointLight        = 0,
    LightType_DirectionalLight  = 1,
};

/// @brief A PointLight contains settings for rendering a point light.
struct BPointLight
{
    glm::vec3 LightColor;
    float LightIntensity;
    float LightRadius;
};

/// @brief A DirectionalLight contains settings for rendering a directional light.
struct BDirectionalLight
{
    glm::vec3 LightColor;
    float LightIntensity;
};

/// @brief A Light represents a virtual light in the world.
class BLight : public BSceneObject
{
public:
    BLight();
    BLight(std::string const& ObjectName);

    virtual SceneObjectType GetSceneObjectType() const override { return SOType_Light; }

public:
    bool CastShadows = false;
    LightType Type = LightType_PointLight;
    union BLightData
    {
        BPointLight PointLight;
        BDirectionalLight DirectionalLight;
    } Light;
};

#endif //LIGHT_HPP
