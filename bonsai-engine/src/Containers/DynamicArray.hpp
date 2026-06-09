#pragma once
#ifndef DYNAMIC_ARRAY_HPP
#define DYNAMIC_ARRAY_HPP

#include <vector>
#include <BonsaiBase.hpp>

template<typename Type>
using DynamicArray = std::pmr::vector<Type>;

#endif //DYNAMIC_ARRAY_HPP
