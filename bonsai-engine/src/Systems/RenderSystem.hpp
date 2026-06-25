#pragma once
#ifndef RENDER_SYSTEM_HPP
#define RENDER_SYSTEM_HPP

#include "Core.hpp"

/// @brief The RenderSystem is used to draw renderable objects in the scene.
class RenderSystem : public ISystem
{
public:
    virtual void Run(World& world, float deltaTime) override;
};

#endif //RENDER_SYSTEM_HPP
