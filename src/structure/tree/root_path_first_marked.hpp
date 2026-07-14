#pragma once

#include <set>
#include <stdexcept>

#include "../../algorithm/tree/heavy_light_decomposition.hpp"

template<int MAX_SIZE>
struct RootPathFirstMarked{
private:
    int _n;
    int root_vertex = 0;
    HeavyLightDecomposition decomposition;
    std::set<int> marked_positions;
    bool built = false;

public:
    explicit RootPathFirstMarked(int n): _n(n), decomposition(n){
        if(n < 0 || MAX_SIZE < n) throw std::runtime_error("library assertion fault: range violation (constructor).");
    }

    void add_edge(int left, int right){ decomposition.add_edge(left, right); }
    void build(int root = 0){
        decomposition.build(root);
        root_vertex = root;
        built = true;
    }

    bool toggle(int vertex){
        if(!built) throw std::runtime_error("library assertion fault: not built (toggle).");
        const int position = decomposition.in(vertex);
        const auto iterator = marked_positions.find(position);
        if(iterator == marked_positions.end()){
            marked_positions.insert(position);
            return true;
        }
        marked_positions.erase(iterator);
        return false;
    }

    int first_marked(int vertex) const{
        if(!built) throw std::runtime_error("library assertion fault: not built (first_marked).");
        for(const auto [left, right]: decomposition.path_query(root_vertex, vertex, true)){
            const auto iterator = marked_positions.lower_bound(left);
            if(iterator != marked_positions.end() && *iterator < right){
                return decomposition[*iterator];
            }
        }
        return -1;
    }
};
