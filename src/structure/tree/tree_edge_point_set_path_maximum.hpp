#ifndef CPPLIB_SRC_STRUCTURE_TREE_TREE_EDGE_POINT_SET_PATH_MAXIMUM_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_TREE_TREE_EDGE_POINT_SET_PATH_MAXIMUM_HPP_INCLUDED

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../algorithm/tree/decomposition/heavy_light_decomposition.hpp"
#include "../segtree/segtree.hpp"
#include "../types/common_monoids.hpp"

template<class Weight, int MAX_SIZE>
struct TreeEdgePointSetPathMaximum{
private:
    struct Edge{
        int left;
        int right;
        Weight weight;
    };

    int _n;
    HeavyLightDecomposition decomposition;
    Segtree<MaxMonoid<Weight>{}, MAX_SIZE> maximum;
    std::vector<Edge> edges;
    bool built = false;
    static int checked_vertex_count(int vertex_count){
        if(vertex_count < 0 || MAX_SIZE < vertex_count)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        return vertex_count;
    }


    int child_of_edge(const Edge& edge) const{
        return decomposition.depth(edge.left) > decomposition.depth(edge.right)
            ? edge.left
            : edge.right;
    }

public:
    explicit TreeEdgePointSetPathMaximum(int vertex_count):
        _n(checked_vertex_count(vertex_count)), decomposition(_n), maximum(_n){
        edges.reserve(static_cast<std::size_t>(std::max(0, _n - 1)));
    }

    int size() const{
        return _n;
    }

    int add_edge(int left, int right, const Weight& weight){
        if(built)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: already built (add_edge)."
            );
        }
        decomposition.add_edge(left, right);
        edges.push_back({left, right, weight});
        return static_cast<int>(edges.size()) - 1;
    }

    void build(int root = 0){
        built = false;
        decomposition.build(root);
        for(const Edge& edge: edges){
            maximum.set(decomposition.in(child_of_edge(edge)), edge.weight);
        }
        built = true;
    }

    void set_edge(int edge_index, const Weight& weight){
        if(!built)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: not built (set_edge)."
            );
        }
        if(edge_index < 0 || static_cast<int>(edges.size()) <= edge_index)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (set_edge)."
            );
        }
        Edge& edge = edges[static_cast<std::size_t>(edge_index)];
        edge.weight = weight;
        maximum.set(decomposition.in(child_of_edge(edge)), weight);
    }

    Weight path_maximum(int left, int right) const{
        if(!built)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: not built (path_maximum)."
            );
        }
        const auto monoid = MaxMonoid<Weight>{};
        Weight answer = monoid.e();
        for(const auto [range_left, range_right]:
            decomposition.path_query(left, right, false)){
            answer = monoid.op(
                answer,
                maximum.prod(range_left, range_right)
            );
        }
        return answer;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_TREE_TREE_EDGE_POINT_SET_PATH_MAXIMUM_HPP_INCLUDED
