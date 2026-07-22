#ifndef CPPLIB_SRC_ALGORITHM_MATCHING_MAXIMUM_WEIGHT_GENERAL_MATCHING_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATCHING_MAXIMUM_WEIGHT_GENERAL_MATCHING_HPP_INCLUDED

#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "minimum_weight_general_matching_internal.hpp"

template<class T>
struct MaximumWeightGeneralMatchingEdge{
    int from;
    int to;
    T weight;
};

template<class T>
struct MaximumWeightGeneralMatchingResult{
    int size;
    T weight;
    std::vector<int> match;
};

template<class T>
MaximumWeightGeneralMatchingResult<T> maximum_weight_general_matching(
    int n,
    const std::vector<MaximumWeightGeneralMatchingEdge<T>>& edges
){
    static_assert(std::is_integral_v<T> && sizeof(T) <= sizeof(long long));
    static_assert(!std::is_same_v<std::remove_cv_t<T>, bool>);
    if(n < 0 || std::numeric_limits<int>::max() / 2 < n)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (maximum_weight_general_matching).");
    }

    std::vector<std::vector<char>> exists(
        static_cast<std::size_t>(n),
        std::vector<char>(static_cast<std::size_t>(n), 0)
    );
    std::vector<std::vector<T>> weights(
        static_cast<std::size_t>(n),
        std::vector<T>(static_cast<std::size_t>(n), T(0))
    );
    for(const auto& edge: edges){
        if(edge.from < 0 || n <= edge.from || edge.to < 0 || n <= edge.to)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (maximum_weight_general_matching).");
        }
        if(edge.from == edge.to) continue;
        const std::size_t from = static_cast<std::size_t>(edge.from);
        const std::size_t to = static_cast<std::size_t>(edge.to);
        if(!exists[from][to] || weights[from][to] < edge.weight){
            exists[from][to] = 1;
            exists[to][from] = 1;
            weights[from][to] = edge.weight;
            weights[to][from] = edge.weight;
        }
    }

    using minimum_weight_general_matching_internal::Wide;
    using minimum_weight_general_matching_internal::WeightedEdge;
    std::vector<WeightedEdge> weighted_edges;
    weighted_edges.reserve(edges.size());
    for(int from = 0; from < n; ++from){
        for(int to = from + 1; to < n; ++to){
            if(!exists[static_cast<std::size_t>(from)][static_cast<std::size_t>(to)]) continue;
            weighted_edges.push_back({
                from,
                to,
                static_cast<Wide>(weights[static_cast<std::size_t>(from)][static_cast<std::size_t>(to)])
            });
        }
    }
    if(weighted_edges.size() > static_cast<std::size_t>(std::numeric_limits<int>::max() / 2))[[unlikely]]{
        throw std::runtime_error("library assertion fault: too many edges (maximum_weight_general_matching).");
    }

    minimum_weight_general_matching_internal::WeightedBlossom solver(n, std::move(weighted_edges));
    MaximumWeightGeneralMatchingResult<T> result;
    result.match = solver.solve(false);
    result.size = 0;
    Wide total_weight = 0;
    for(int vertex = 0; vertex < n; ++vertex){
        const int other = result.match[static_cast<std::size_t>(vertex)];
        if(vertex < other){
            ++result.size;
            total_weight += static_cast<Wide>(
                weights[static_cast<std::size_t>(vertex)][static_cast<std::size_t>(other)]
            );
        }
    }

    const Wide minimum = static_cast<Wide>(std::numeric_limits<T>::lowest());
    const Wide maximum = static_cast<Wide>(std::numeric_limits<T>::max());
    if(total_weight < minimum || maximum < total_weight)[[unlikely]]{
        throw std::overflow_error("library assertion fault: overflow (maximum_weight_general_matching).");
    }
    result.weight = static_cast<T>(total_weight);
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_MATCHING_MAXIMUM_WEIGHT_GENERAL_MATCHING_HPP_INCLUDED
