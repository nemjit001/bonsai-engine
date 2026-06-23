#pragma once
#ifndef LEVEL_HPP
#define LEVEL_HPP

#include <entt/entt.hpp>
#include "AssetType.hpp"

/// @brief A Level asset is a snapshot of scene entities that can be restored from, or loaded into the game world.
class Level : public IAssetType
{
public:
    entt::snapshot snapshot;
};

#endif //LEVEL_HPP
