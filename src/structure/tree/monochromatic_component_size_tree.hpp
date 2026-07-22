#ifndef CPPLIB_SRC_STRUCTURE_TREE_MONOCHROMATIC_COMPONENT_SIZE_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_TREE_MONOCHROMATIC_COMPONENT_SIZE_TREE_HPP_INCLUDED

#include <vector>

#include "dynamic_monochromatic_tree.hpp"
#include "../types/common_monoids.hpp"

template<int MAX_SIZE>
struct MonochromaticComponentSizeTree{
private:
    DynamicMonochromaticTree<AddMonoid<int>{}, MAX_SIZE> tree;

public:
    explicit MonochromaticComponentSizeTree(int n):
        tree(std::vector<int>(static_cast<std::size_t>(n), 1)){}
    void add_edge(int left, int right){ tree.add_edge(left, right); }
    void build(){ tree.build(); }
    bool toggle(int vertex){ return tree.toggle(vertex); }
    int component_size(int vertex){ return tree.component_prod(vertex); }
};

#endif  // CPPLIB_SRC_STRUCTURE_TREE_MONOCHROMATIC_COMPONENT_SIZE_TREE_HPP_INCLUDED
