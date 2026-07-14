#pragma once

#include <algorithm>
#include <stdexcept>
#include <utility>

#include "heavy_light_decomposition.hpp"
#include "../../structure/other/fenwick_tree.hpp"

template<int MAX_SIZE>
struct GrassPlantingTree{
private:
    int _n;
    HeavyLightDecomposition decomposition;
    FenwickTree<int, MAX_SIZE + 1> difference;
    bool built = false;

public:
    explicit GrassPlantingTree(int n):
        _n(n), decomposition(n), difference(n + 1){}

    int size() const{ return _n; }

    void add_edge(int left, int right){
        decomposition.add_edge(left, right);
    }

    void build(int root = 0){
        decomposition.build(root);
        built = true;
    }

    void plant_path(int left, int right){
        if(!built) throw std::runtime_error("library assertion fault: not built (plant_path).");
        for(const auto [range_left, range_right]:
            decomposition.path_query(left, right, false)){
            difference.add(range_left, 1);
            difference.add(range_right, -1);
        }
    }

    int edge_plant_count(int left, int right) const{
        if(!built) throw std::runtime_error("library assertion fault: not built (edge_plant_count).");
        const int child = decomposition.depth(left) > decomposition.depth(right)
            ? left
            : right;
        return difference.prefix_sum(decomposition.in(child) + 1);
    }
};
