#ifndef CPPLIB_SRC_ALGORITHM_MATCHING_MINIMUM_WEIGHT_GENERAL_MATCHING_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATCHING_MINIMUM_WEIGHT_GENERAL_MATCHING_HPP_INCLUDED

#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "minimum_weight_general_matching_internal.hpp"

template<class T>
struct MinimumWeightGeneralMatchingEdge{
    int from;
    int to;
    T cost;
};

template<class T>
struct MinimumWeightGeneralMatchingResult{
    int size;
    T cost;
    std::vector<int> match;
};

template<class T>
MinimumWeightGeneralMatchingResult<T> minimum_weight_general_matching(
    int n,
    const std::vector<MinimumWeightGeneralMatchingEdge<T>>& edges,
    T inf = std::numeric_limits<T>::max() / 4
){
    static_assert(std::is_integral_v<T> && sizeof(T) <= sizeof(long long));
    static_assert(!std::is_same_v<std::remove_cv_t<T>, bool>);
    if(n < 0 || std::numeric_limits<int>::max() / 2 < n)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (minimum_weight_general_matching).");
    }
    (void)inf;

    std::vector<std::vector<char>> exists(
        static_cast<std::size_t>(n),
        std::vector<char>(static_cast<std::size_t>(n), 0)
    );
    std::vector<std::vector<T>> costs(
        static_cast<std::size_t>(n),
        std::vector<T>(static_cast<std::size_t>(n), T(0))
    );
    for(const auto& edge: edges){
        if(edge.from < 0 || n <= edge.from || edge.to < 0 || n <= edge.to)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (minimum_weight_general_matching).");
        }
        if(edge.from == edge.to) continue;
        const std::size_t from = static_cast<std::size_t>(edge.from);
        const std::size_t to = static_cast<std::size_t>(edge.to);
        if(!exists[from][to] || edge.cost < costs[from][to]){
            exists[from][to] = 1;
            exists[to][from] = 1;
            costs[from][to] = edge.cost;
            costs[to][from] = edge.cost;
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
                -static_cast<Wide>(costs[static_cast<std::size_t>(from)][static_cast<std::size_t>(to)])
            });
        }
    }

    if(weighted_edges.size() > static_cast<std::size_t>(std::numeric_limits<int>::max() / 2))[[unlikely]]{
        throw std::runtime_error("library assertion fault: too many edges (minimum_weight_general_matching).");
    }
    minimum_weight_general_matching_internal::WeightedBlossom solver(n, std::move(weighted_edges));
    MinimumWeightGeneralMatchingResult<T> result;
    result.match = solver.solve();
    result.size = 0;
    Wide total_cost = 0;
    for(int vertex = 0; vertex < n; ++vertex){
        const int other = result.match[static_cast<std::size_t>(vertex)];
        if(vertex < other){
            ++result.size;
            total_cost += static_cast<Wide>(costs[static_cast<std::size_t>(vertex)][static_cast<std::size_t>(other)]);
        }
    }

    const Wide minimum = static_cast<Wide>(std::numeric_limits<T>::lowest());
    const Wide maximum = static_cast<Wide>(std::numeric_limits<T>::max());
    if(total_cost < minimum || maximum < total_cost)[[unlikely]]{
        throw std::overflow_error("library assertion fault: overflow (minimum_weight_general_matching).");
    }
    result.cost = static_cast<T>(total_cost);
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_MATCHING_MINIMUM_WEIGHT_GENERAL_MATCHING_HPP_INCLUDED
