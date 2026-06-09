#pragma once
#ifndef DYNAMIC_ARRAY_HPP
#define DYNAMIC_ARRAY_HPP

#include <vector>

template<typename Type>
using DynamicArray = std::pmr::vector<Type>;

#endif //DYNAMIC_ARRAY_HPP
