#ifndef CPPLIB_SRC_STRUCTURE_TREE_MARKED_TREE_DIAMETER_QUERIES_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_TREE_MARKED_TREE_DIAMETER_QUERIES_HPP_INCLUDED

#include <optional>

#include "dynamic_marked_tree_diameter.hpp"

template<class Weight, int MAX_SIZE>
struct MarkedTreeDiameterQueries{
private:
    DynamicMarkedTreeDiameter<Weight, MAX_SIZE> tree;

public:
    explicit MarkedTreeDiameterQueries(int n): tree(n){}
    void add_edge(int left, int right, Weight weight){ tree.add_edge(left, right, weight); }
    void build(bool initially_marked = true){ tree.build(initially_marked); }
    bool toggle(int vertex){ return tree.toggle(vertex); }
    std::optional<Weight> diameter() const{ return tree.diameter(); }
};

#endif  // CPPLIB_SRC_STRUCTURE_TREE_MARKED_TREE_DIAMETER_QUERIES_HPP_INCLUDED
