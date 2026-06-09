#pragma once
#ifndef STATIC_ARRAY_HPP
#define STATIC_ARRAY_HPP

#include <array>
#include <BonsaiBase.hpp>

template<typename Type, usize Size>
using StaticArray = std::array<Type, Size>

#endif //STATIC_ARRAY_HPP
