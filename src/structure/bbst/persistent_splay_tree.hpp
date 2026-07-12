#pragma once

#include "persistent_red_black_tree.hpp"

template<class Key, int MAX_NODE, int MAX_VERSION, class Compare = std::less<Key>>
using PersistentSplaySet = PersistentRedBlackSet<Key, MAX_NODE, MAX_VERSION, Compare>;
