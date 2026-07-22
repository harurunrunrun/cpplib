#ifndef CPPLIB_SRC_APPROXIMATE_ROUTING_TREE_TSP_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_ROUTING_TREE_TSP_HPP_INCLUDED

#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "distance_matrix.hpp"

namespace approximate::routing {

namespace internal {

template<class DistanceMatrix>
std::vector<int> dense_prim_parents(
    const DistanceMatrix& distance,
    std::size_t root
){
    const std::size_t n = validate_distance_matrix(distance);
    if(n == 0){
        if(root != 0) throw std::out_of_range("root is outside an empty matrix");
        return {};
    }
    if(root >= n) throw std::out_of_range("root vertex is outside the matrix");

    using Cost = distance_cost_t<DistanceMatrix>;
    std::vector<int> parent(n, -1);
    std::vector<Cost> best(n);
    std::vector<unsigned char> has_best(n, 0);
    std::vector<unsigned char> used(n, 0);
    used[root] = 1;
    for(std::size_t vertex = 0; vertex < n; ++vertex){
        if(vertex == root) continue;
        best[vertex] = distance[root][vertex];
        has_best[vertex] = 1;
        parent[vertex] = static_cast<int>(root);
    }

    for(std::size_t iteration = 1; iteration < n; ++iteration){
        std::size_t selected = n;
        for(std::size_t vertex = 0; vertex < n; ++vertex){
            if(used[vertex] != 0 || has_best[vertex] == 0) continue;
            if(selected == n || best[vertex] < best[selected]) selected = vertex;
        }
        if(selected == n){
            throw std::invalid_argument("distance graph is disconnected");
        }
        used[selected] = 1;
        for(std::size_t vertex = 0; vertex < n; ++vertex){
            if(used[vertex] != 0) continue;
            if(has_best[vertex] == 0 || distance[selected][vertex] < best[vertex]){
                best[vertex] = distance[selected][vertex];
                has_best[vertex] = 1;
                parent[vertex] = static_cast<int>(selected);
            }
        }
    }
    return parent;
}

inline std::vector<std::vector<int>> tree_adjacency(
    const std::vector<int>& parent
){
    std::vector<std::vector<int>> tree(parent.size());
    for(std::size_t vertex = 0; vertex < parent.size(); ++vertex){
        if(parent[vertex] < 0) continue;
        const std::size_t other = static_cast<std::size_t>(parent[vertex]);
        tree[vertex].push_back(static_cast<int>(other));
        tree[other].push_back(static_cast<int>(vertex));
    }
    for(auto& neighbors : tree) std::sort(neighbors.begin(), neighbors.end());
    return tree;
}

inline std::vector<int> preorder_of_tree(
    const std::vector<std::vector<int>>& tree,
    std::size_t root
){
    if(tree.empty()) return {};
    std::vector<int> order;
    order.reserve(tree.size());
    std::vector<std::pair<int, int>> stack;
    stack.emplace_back(static_cast<int>(root), -1);
    while(!stack.empty()){
        const auto [vertex, parent] = stack.back();
        stack.pop_back();
        order.push_back(vertex);
        const auto& neighbors = tree[static_cast<std::size_t>(vertex)];
        for(auto iterator = neighbors.rbegin(); iterator != neighbors.rend(); ++iterator){
            if(*iterator != parent) stack.emplace_back(*iterator, vertex);
        }
    }
    return order;
}

}  // namespace internal

template<class DistanceMatrix>
std::vector<int> minimum_spanning_tree_parents(
    const DistanceMatrix& distance,
    std::size_t root = 0
){
    return internal::dense_prim_parents(distance, root);
}

template<class DistanceMatrix>
std::vector<int> double_tree_tsp_tour(
    const DistanceMatrix& distance,
    std::size_t root = 0
){
    const std::vector<int> parent = internal::dense_prim_parents(distance, root);
    return internal::preorder_of_tree(internal::tree_adjacency(parent), root);
}

template<class DistanceMatrix>
std::vector<int> christofides_tsp_tour(
    const DistanceMatrix& distance,
    std::size_t root = 0,
    std::size_t maximum_odd_vertices = 22
){
    using Cost = internal::distance_cost_t<DistanceMatrix>;
    const std::size_t n = internal::validate_distance_matrix(distance);
    if(n == 0){
        if(root != 0) throw std::out_of_range("root is outside an empty matrix");
        return {};
    }
    if(root >= n) throw std::out_of_range("root vertex is outside the matrix");
    if(maximum_odd_vertices > 24){
        throw std::invalid_argument("maximum_odd_vertices must not exceed 24");
    }

    const std::vector<int> parent = internal::dense_prim_parents(distance, root);
    std::vector<int> degree(n, 0);
    std::vector<std::pair<int, int>> edges;
    edges.reserve(2 * n);
    for(std::size_t vertex = 0; vertex < n; ++vertex){
        if(parent[vertex] < 0) continue;
        edges.emplace_back(static_cast<int>(vertex), parent[vertex]);
        ++degree[vertex];
        ++degree[static_cast<std::size_t>(parent[vertex])];
    }

    std::vector<int> odd;
    for(std::size_t vertex = 0; vertex < n; ++vertex){
        if((degree[vertex] & 1) != 0) odd.push_back(static_cast<int>(vertex));
    }
    if(odd.size() > maximum_odd_vertices){
        throw std::length_error("too many odd MST vertices for exact matching");
    }

    const std::size_t state_count = std::size_t{1} << odd.size();
    std::vector<Cost> memo(state_count);
    std::vector<unsigned char> seen(state_count, 0);
    std::vector<signed char> choice(state_count, -1);
    const auto solve = [&](const auto& self, std::uint64_t mask) -> Cost {
        if(mask == 0) return Cost{};
        const std::size_t state = static_cast<std::size_t>(mask);
        if(seen[state] != 0) return memo[state];
        seen[state] = 1;
        const std::size_t first = static_cast<std::size_t>(std::countr_zero(mask));
        const std::uint64_t without_first = mask & ~(std::uint64_t{1} << first);
        bool initialized = false;
        Cost best{};
        for(std::size_t second = first + 1;
            second < odd.size();
            ++second
        ){
            const std::uint64_t second_bit = std::uint64_t{1} << second;
            if((without_first & second_bit) == 0) continue;
            const Cost candidate = distance[
                static_cast<std::size_t>(odd[first])
            ][static_cast<std::size_t>(odd[second])] + self(
                self,
                without_first & ~second_bit
            );
            if(!initialized || candidate < best){
                initialized = true;
                best = candidate;
                choice[state] = static_cast<signed char>(second);
            }
        }
        memo[state] = best;
        return best;
    };
    const std::uint64_t full_mask = (std::uint64_t{1} << odd.size()) - 1;
    static_cast<void>(solve(solve, full_mask));
    std::uint64_t mask = full_mask;
    while(mask != 0){
        const std::size_t first = static_cast<std::size_t>(std::countr_zero(mask));
        const int second_value = choice[static_cast<std::size_t>(mask)];
        if(second_value < 0) throw std::logic_error("perfect matching reconstruction failed");
        const std::size_t second = static_cast<std::size_t>(second_value);
        edges.emplace_back(
            odd[first],
            odd[second]
        );
        mask &= ~(std::uint64_t{1} << first);
        mask &= ~(std::uint64_t{1} << second);
    }

    std::vector<std::vector<std::pair<int, std::size_t>>> graph(n);
    for(std::size_t edge_id = 0; edge_id < edges.size(); ++edge_id){
        const auto [from, to] = edges[edge_id];
        graph[static_cast<std::size_t>(from)].emplace_back(to, edge_id);
        graph[static_cast<std::size_t>(to)].emplace_back(from, edge_id);
    }
    std::vector<unsigned char> edge_used(edges.size(), 0);
    std::vector<std::size_t> cursor(n, 0);
    std::vector<int> stack{static_cast<int>(root)};
    std::vector<int> euler;
    euler.reserve(edges.size() + 1);
    while(!stack.empty()){
        const int vertex_value = stack.back();
        const std::size_t vertex = static_cast<std::size_t>(vertex_value);
        while(
            cursor[vertex] < graph[vertex].size()
            && edge_used[graph[vertex][cursor[vertex]].second] != 0
        ) ++cursor[vertex];
        if(cursor[vertex] == graph[vertex].size()){
            euler.push_back(vertex_value);
            stack.pop_back();
        }else{
            const auto [next, edge_id] = graph[vertex][cursor[vertex]];
            ++cursor[vertex];
            edge_used[edge_id] = 1;
            stack.push_back(next);
        }
    }
    std::reverse(euler.begin(), euler.end());

    std::vector<unsigned char> vertex_used(n, 0);
    std::vector<int> tour;
    tour.reserve(n);
    for(const int vertex_value : euler){
        const std::size_t vertex = static_cast<std::size_t>(vertex_value);
        if(vertex_used[vertex] != 0) continue;
        vertex_used[vertex] = 1;
        tour.push_back(vertex_value);
    }
    return tour;
}

}  // namespace approximate::routing

#endif  // CPPLIB_SRC_APPROXIMATE_ROUTING_TREE_TSP_HPP_INCLUDED
