#ifndef CPPLIB_SRC_STRUCTURE_TREE_NEAREST_MARKED_TREE_QUERIES_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_TREE_NEAREST_MARKED_TREE_QUERIES_HPP_INCLUDED

#include <optional>

#include "dynamic_nearest_marked_vertex.hpp"

template<class Weight, int MAX_SIZE>
struct NearestMarkedTreeQueries{
private:
    DynamicNearestMarkedVertex<Weight, MAX_SIZE> tree;

public:
    explicit NearestMarkedTreeQueries(int n): tree(n){}
    void add_edge(int left, int right, Weight weight = Weight{1}){ tree.add_edge(left, right, weight); }
    void build(){ tree.build(); }
    void toggle(int vertex){ tree.toggle(vertex); }
    std::optional<Weight> nearest_distance(int vertex) const{ return tree.nearest_distance(vertex); }
};

#endif  // CPPLIB_SRC_STRUCTURE_TREE_NEAREST_MARKED_TREE_QUERIES_HPP_INCLUDED
