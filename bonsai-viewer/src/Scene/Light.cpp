#include "Light.hpp"

BLight::BLight()
    :
    BLight("Light")
{
    //
}

BLight::BLight(std::string const& ObjectName)
    :
    BSceneObject(ObjectName)
{
    //
}
