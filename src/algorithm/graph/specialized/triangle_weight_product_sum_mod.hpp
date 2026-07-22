#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_TRIANGLE_WEIGHT_PRODUCT_SUM_MOD_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_TRIANGLE_WEIGHT_PRODUCT_SUM_MOD_HPP_INCLUDED

#include <stdexcept>
#include <utility>
#include <vector>

#include "enumerate_triangles.hpp"

inline long long triangle_weight_product_sum_mod(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges,
    const std::vector<long long>& weights,
    long long modulus
){
    if(vertex_count < 0 ||
       weights.size() != static_cast<std::size_t>(vertex_count) ||
       modulus <= 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (triangle_weight_product_sum_mod)."
        );
    }

    std::vector<long long> normalized(weights.size());
    for(std::size_t index = 0; index < weights.size(); ++index){
        normalized[index] = weights[index] % modulus;
        if(normalized[index] < 0) normalized[index] += modulus;
    }

    long long result = 0;
    enumerate_triangles(vertex_count, edges, [&](int first, int second, int third){
        __int128 product = normalized[static_cast<std::size_t>(first)];
        product = product * normalized[static_cast<std::size_t>(second)] % modulus;
        product = product * normalized[static_cast<std::size_t>(third)] % modulus;
        result = static_cast<long long>((static_cast<__int128>(result) + product) % modulus);
    });
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_TRIANGLE_WEIGHT_PRODUCT_SUM_MOD_HPP_INCLUDED
