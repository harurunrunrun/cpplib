#ifndef CPPLIB_SRC_APPROXIMATE_ROUTING_ONE_TREE_HEURISTICS_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_ROUTING_ONE_TREE_HEURISTICS_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "distance_matrix.hpp"

namespace approximate::routing {

struct OneTreeResult {
    long double lower_bound = 0;
    long double modified_cost = 0;
    std::vector<std::pair<int, int>> edges;
    std::vector<int> degree;
};

template<class DistanceMatrix>
OneTreeResult minimum_one_tree(
    const DistanceMatrix& distance,
    std::size_t root = 0,
    const std::vector<long double>& vertex_penalty = {}
) {
    const std::size_t n = internal::validate_distance_matrix(distance);
    if(n < 3){
        throw std::invalid_argument("a 1-tree needs at least three vertices");
    }
    if(n > static_cast<std::size_t>(std::numeric_limits<int>::max())){
        throw std::length_error("vertex count does not fit in int");
    }
    if(root >= n) throw std::out_of_range("root is outside the distance matrix");
    if(!vertex_penalty.empty() && vertex_penalty.size() != n){
        throw std::invalid_argument("vertex_penalty has wrong size");
    }
    for(const long double value : vertex_penalty){
        if(!std::isfinite(value)){
            throw std::invalid_argument("vertex_penalty must be finite");
        }
    }
    const auto penalty = [&](std::size_t vertex) {
        return vertex_penalty.empty() ? 0.0L : vertex_penalty[vertex];
    };
    const auto weight = [&](std::size_t first, std::size_t second) {
        return static_cast<long double>(distance[first][second])
             + penalty(first) + penalty(second);
    };

    const long double infinity = std::numeric_limits<long double>::infinity();
    std::vector<long double> best(n, infinity);
    std::vector<int> parent(n, -1);
    std::vector<unsigned char> used(n, 0);
    std::size_t start = root == 0 ? 1 : 0;
    best[start] = 0;

    OneTreeResult result;
    result.degree.assign(n, 0);
    for(std::size_t iteration = 0; iteration + 1 < n; ++iteration){
        std::size_t vertex = n;
        for(std::size_t candidate = 0; candidate < n; ++candidate){
            if(candidate == root || used[candidate]) continue;
            if(vertex == n || best[candidate] < best[vertex]
               || (best[candidate] == best[vertex] && candidate < vertex)){
                vertex = candidate;
            }
        }
        used[vertex] = 1;
        if(parent[vertex] >= 0){
            const int p = parent[vertex];
            result.edges.emplace_back(p, static_cast<int>(vertex));
            ++result.degree[static_cast<std::size_t>(p)];
            ++result.degree[vertex];
            result.modified_cost +=
                weight(static_cast<std::size_t>(p), vertex);
        }
        for(std::size_t candidate = 0; candidate < n; ++candidate){
            if(candidate == root || used[candidate]) continue;
            const long double candidate_weight = weight(vertex, candidate);
            if(candidate_weight < best[candidate]
               || (candidate_weight == best[candidate]
                   && static_cast<int>(vertex) < parent[candidate])){
                best[candidate] = candidate_weight;
                parent[candidate] = static_cast<int>(vertex);
            }
        }
    }

    std::size_t first = n;
    std::size_t second = n;
    for(std::size_t vertex = 0; vertex < n; ++vertex){
        if(vertex == root) continue;
        if(first == n || weight(root, vertex) < weight(root, first)
           || (weight(root, vertex) == weight(root, first) && vertex < first)){
            second = first;
            first = vertex;
        }else if(second == n || weight(root, vertex) < weight(root, second)
                 || (weight(root, vertex) == weight(root, second)
                     && vertex < second)){
            second = vertex;
        }
    }
    for(const std::size_t vertex : {first, second}){
        result.edges.emplace_back(static_cast<int>(root), static_cast<int>(vertex));
        ++result.degree[root];
        ++result.degree[vertex];
        result.modified_cost += weight(root, vertex);
    }
    long double penalty_sum = 0;
    for(std::size_t vertex = 0; vertex < n; ++vertex){
        penalty_sum += penalty(vertex);
    }
    result.lower_bound = result.modified_cost - 2 * penalty_sum;
    return result;
}

struct HeldKarpSubgradientResult {
    long double lower_bound = -std::numeric_limits<long double>::infinity();
    std::vector<long double> vertex_penalty;
    std::vector<std::pair<int, int>> one_tree_edges;
    std::vector<int> one_tree_degree;
    std::size_t iterations = 0;
    bool is_tour = false;
};

template<class DistanceMatrix>
HeldKarpSubgradientResult held_karp_subgradient_heuristic(
    const DistanceMatrix& distance,
    long double upper_bound,
    std::size_t maximum_iterations = 200,
    long double initial_step_factor = 2.0L,
    std::size_t root = 0
) {
    const std::size_t n = internal::validate_distance_matrix(distance);
    if(n < 3){
        throw std::invalid_argument(
            "Held-Karp subgradient needs at least three vertices"
        );
    }
    if(root >= n) throw std::out_of_range("root is outside the distance matrix");
    if(maximum_iterations == 0){
        throw std::invalid_argument("maximum_iterations must be positive");
    }
    if(!(initial_step_factor > 0) || !std::isfinite(initial_step_factor)){
        throw std::invalid_argument(
            "initial_step_factor must be finite and positive"
        );
    }
    if(std::isnan(upper_bound)){
        throw std::invalid_argument("upper_bound must not be NaN");
    }

    HeldKarpSubgradientResult result;
    std::vector<long double> penalty(n, 0);
    long double factor = initial_step_factor;
    std::size_t iterations_without_improvement = 0;
    for(std::size_t iteration = 0; iteration < maximum_iterations; ++iteration){
        const OneTreeResult tree = minimum_one_tree(distance, root, penalty);
        ++result.iterations;
        if(tree.lower_bound > result.lower_bound){
            result.lower_bound = tree.lower_bound;
            result.vertex_penalty = penalty;
            result.one_tree_edges = tree.edges;
            result.one_tree_degree = tree.degree;
            iterations_without_improvement = 0;
        }else{
            ++iterations_without_improvement;
        }

        long double norm_squared = 0;
        for(const int degree : tree.degree){
            const long double subgradient =
                static_cast<long double>(degree - 2);
            norm_squared += subgradient * subgradient;
        }
        if(norm_squared == 0){
            result.is_tour = true;
            break;
        }

        long double step = factor / std::sqrt(norm_squared);
        if(std::isfinite(upper_bound) && upper_bound > tree.lower_bound){
            step = factor * (upper_bound - tree.lower_bound) / norm_squared;
        }
        for(std::size_t vertex = 0; vertex < n; ++vertex){
            penalty[vertex] +=
                step * static_cast<long double>(tree.degree[vertex] - 2);
        }
        if(iterations_without_improvement >= 20){
            factor *= 0.5L;
            iterations_without_improvement = 0;
        }
    }
    return result;
}

}  // namespace approximate::routing

#endif  // CPPLIB_SRC_APPROXIMATE_ROUTING_ONE_TREE_HEURISTICS_HPP_INCLUDED
