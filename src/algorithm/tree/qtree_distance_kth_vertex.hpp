#pragma once

#include "static_tree_path_fold.hpp"
#include "../../structure/types/common_monoids.hpp"

template<class Weight, int MAX_SIZE>
struct QTreeDistanceKthVertex{
private:
    StaticTreePathFold<AddMonoid<Weight>{}, MAX_SIZE> tree;

public:
    explicit QTreeDistanceKthVertex(int n): tree(n){}
    int size() const{ return tree.size(); }
    void add_edge(int left, int right, Weight weight){ tree.add_edge(left, right, weight); }
    void build(int root = 0){ tree.build(root); }
    Weight distance(int left, int right) const{ return tree.prod(left, right); }
    int kth_vertex(int left, int right, int order) const{
        return tree.kth_vertex_on_path(left, right, order);
    }
};
