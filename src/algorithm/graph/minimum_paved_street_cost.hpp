#pragma once

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

#include "minimum_spanning_tree_cost.hpp"

inline long long minimum_paved_street_cost(
    long long unit_price,
    int vertex_count,
    const std::vector<KruskalEdge<long long>>& streets
){
    if(unit_price < 0)[[unlikely]]{
        throw std::invalid_argument("minimum_paved_street_cost negative price");
    }
    for(const auto& street: streets){
        if(street.cost < 0)[[unlikely]]{
            throw std::invalid_argument(
                "minimum_paved_street_cost negative length"
            );
        }
    }
    const long long length =
        minimum_spanning_tree_cost(vertex_count, streets);
    const __int128_t answer = static_cast<__int128_t>(unit_price) * length;
    if(answer > std::numeric_limits<long long>::max())[[unlikely]]{
        throw std::overflow_error("minimum_paved_street_cost overflow");
    }
    return static_cast<long long>(answer);
}
