#pragma once
#ifndef STATIC_ARRAY_HPP
#define STATIC_ARRAY_HPP

#include <array>

template<typename Type, std::size_t Size>
using StaticArray = std::array<Type, Size>

#endif //STATIC_ARRAY_HPP
