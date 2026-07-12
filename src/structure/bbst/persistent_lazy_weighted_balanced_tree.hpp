#pragma once

#include "persistent_lazy_reversible_splay_tree.hpp"

template<auto MonoidActLen, int MAX_NODE, int MAX_VERSION>
using PersistentLazyWeightedBalancedTree = PersistentLazyReversibleSplayTree<MonoidActLen, MAX_NODE, MAX_VERSION>;
