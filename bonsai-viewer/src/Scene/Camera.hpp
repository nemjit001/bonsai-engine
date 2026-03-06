#pragma once
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "SceneObject.hpp"

/// @brief A Camera represents a virtual camera in the world.
class BCamera : public BSceneObject
{
public:
    BCamera();
    BCamera(std::string const& ObjectName);
};

#endif //CAMERA_HPP
