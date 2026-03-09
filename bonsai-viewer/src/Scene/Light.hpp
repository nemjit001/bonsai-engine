#pragma once
#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "SceneObject.hpp"

class BLight : public BSceneObject
{
public:
    BLight();
    BLight(std::string const& ObjectName);
};

#endif //LIGHT_HPP
