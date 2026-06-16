#pragma once
#ifndef HASH_MAP_HPP
#define HASH_MAP_HPP

#include <unordered_map>

template<typename Key, typename Value>
using HashMap = std::pmr::unordered_map<Key, Value>;

#endif //HASH_MAP_HPP
