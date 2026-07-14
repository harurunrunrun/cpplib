#pragma once

#include <cstddef>
#include <stdexcept>
#include <vector>

#include "kruskal_mst.hpp"

struct CatFeastDistance{
    int first;
    int second;
    long long distance;
};

inline bool can_feed_all_cats(
    long long milk,
    int cat_count,
    const std::vector<CatFeastDistance>& distances
){
    if(milk < 0 || cat_count <= 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (can_feed_all_cats)."
        );
    }

    std::vector<KruskalEdge<__int128_t>> edges;
    edges.reserve(distances.size());
    for(const auto& edge: distances){
        if(edge.first < 0 || cat_count <= edge.first || edge.second < 0 ||
           cat_count <= edge.second || edge.distance < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (can_feed_all_cats)."
            );
        }
        edges.push_back({edge.first, edge.second, edge.distance});
    }

    const auto tree = kruskal_mst<__int128_t>(cat_count, edges);
    if(!tree.connected)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: disconnected graph (can_feed_all_cats)."
        );
    }
    const __int128_t required = tree.cost + cat_count;
    return required <= static_cast<__int128_t>(milk);
}
