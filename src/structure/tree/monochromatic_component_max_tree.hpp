#ifndef CPPLIB_SRC_STRUCTURE_TREE_MONOCHROMATIC_COMPONENT_MAX_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_TREE_MONOCHROMATIC_COMPONENT_MAX_TREE_HPP_INCLUDED

#include "dynamic_monochromatic_tree.hpp"
#include "../types/common_monoids.hpp"

template<class Value, int MAX_SIZE>
struct MonochromaticComponentMaxTree{
private:
    DynamicMonochromaticTree<MaxMonoid<Value>{}, MAX_SIZE> tree;

public:
    explicit MonochromaticComponentMaxTree(int n): tree(n){}
    void add_edge(int left, int right){ tree.add_edge(left, right); }
    bool set_color(int vertex, bool color){ return tree.set_color(vertex, color); }
    void set_value(int vertex, Value value){ tree.set(vertex, value); }
    void build(){ tree.build(); }
    bool toggle(int vertex){ return tree.toggle(vertex); }
    Value component_maximum(int vertex){ return tree.component_prod(vertex); }
};

#endif  // CPPLIB_SRC_STRUCTURE_TREE_MONOCHROMATIC_COMPONENT_MAX_TREE_HPP_INCLUDED
