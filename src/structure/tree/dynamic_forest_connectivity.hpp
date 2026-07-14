#pragma once

#include "link_cut_tree.hpp"
#include "../types/common_monoids.hpp"

template<int MAX_SIZE>
struct DynamicForestConnectivity{
private:
    LinkCutTree<AddMonoid<int>{}, MAX_SIZE> forest;

public:
    explicit DynamicForestConnectivity(int n): forest(n){}
    bool add_edge(int left, int right){ return forest.link(left, right); }
    bool remove_edge(int left, int right){ return forest.cut(left, right); }
    bool connected(int left, int right){ return forest.connected(left, right); }
};
