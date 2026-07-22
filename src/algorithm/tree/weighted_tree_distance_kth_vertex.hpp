#ifndef CPPLIB_SRC_ALGORITHM_TREE_WEIGHTED_TREE_DISTANCE_KTH_VERTEX_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_WEIGHTED_TREE_DISTANCE_KTH_VERTEX_HPP_INCLUDED

#include "static_tree_path_fold.hpp"
#include "../../structure/types/common_monoids.hpp"

template<class Weight, int MAX_SIZE>
struct WeightedTreeDistanceKthVertex{
private:
    StaticTreePathFold<AddMonoid<Weight>{}, MAX_SIZE> tree;

public:
    explicit WeightedTreeDistanceKthVertex(int vertex_count):
        tree(vertex_count){}

    int size() const{
        return tree.size();
    }

    void add_edge(int left, int right, const Weight& weight){
        tree.add_edge(left, right, weight);
    }

    void build(int root = 0){
        tree.build(root);
    }

    Weight distance(int left, int right) const{
        return tree.prod(left, right);
    }

    int kth_vertex(int left, int right, int order) const{
        return tree.kth_vertex_on_path(left, right, order);
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_TREE_WEIGHTED_TREE_DISTANCE_KTH_VERTEX_HPP_INCLUDED
