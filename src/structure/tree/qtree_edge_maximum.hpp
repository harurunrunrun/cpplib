#pragma once

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../algorithm/tree/heavy_light_decomposition.hpp"
#include "../segtree/segtree.hpp"
#include "../types/common_monoids.hpp"

template<class Weight, int MAX_SIZE>
struct QTreeEdgeMaximum{
private:
    struct Edge{ int left; int right; Weight weight; };
    int _n;
    HeavyLightDecomposition decomposition;
    Segtree<MaxMonoid<Weight>{}, MAX_SIZE> maximum;
    std::vector<Edge> edges;
    bool built = false;

    int child_of_edge(const Edge& edge) const{
        return decomposition.depth(edge.left) > decomposition.depth(edge.right)
            ? edge.left
            : edge.right;
    }

public:
    explicit QTreeEdgeMaximum(int n):
        _n(n), decomposition(n), maximum(n){
        if(n < 0 || MAX_SIZE < n) throw std::runtime_error("library assertion fault: range violation (constructor).");
        edges.reserve(static_cast<std::size_t>(std::max(0, n - 1)));
    }

    int size() const{ return _n; }

    int add_edge(int left, int right, Weight weight){
        if(built) throw std::runtime_error("library assertion fault: already built (add_edge).");
        decomposition.add_edge(left, right);
        edges.push_back({left, right, weight});
        return static_cast<int>(edges.size()) - 1;
    }

    void build(int root = 0){
        decomposition.build(root);
        for(const Edge& edge: edges){
            maximum.set(decomposition.in(child_of_edge(edge)), edge.weight);
        }
        built = true;
    }

    void change_edge(int edge, Weight weight){
        if(!built) throw std::runtime_error("library assertion fault: not built (change_edge).");
        if(edge < 0 || static_cast<int>(edges.size()) <= edge) throw std::runtime_error("library assertion fault: range violation (change_edge).");
        Edge& current = edges[static_cast<std::size_t>(edge)];
        current.weight = weight;
        maximum.set(decomposition.in(child_of_edge(current)), weight);
    }

    Weight path_maximum(int left, int right) const{
        if(!built) throw std::runtime_error("library assertion fault: not built (path_maximum).");
        if(left == right) return Weight{};
        Weight answer = MaxMonoid<Weight>{}.e();
        for(const auto [range_left, range_right]: decomposition.path_query(left, right, false)){
            answer = std::max(answer, maximum.prod(range_left, range_right));
        }
        return answer;
    }
};
