#pragma once
#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

/// @brief A Transform is used to give scene objects a position, rotation, and scale in the world.
class BTransform
{
public:
    BTransform() = default;
    ~BTransform() = default;

    BTransform(BTransform const&) = default;
    BTransform& operator=(BTransform const&) = default;

    BTransform(BTransform&&) = default;
    BTransform& operator=(BTransform&&) = default;
};

#endif //TRANSFORM_HPP
