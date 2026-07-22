#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_SHORTEST_PATH_COMPLEMENT_SHORTEST_PATH_COUNT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_SHORTEST_PATH_COMPLEMENT_SHORTEST_PATH_COUNT_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "complement_shortest_path.hpp"

inline long long complement_shortest_path_count(
    int vertex_count,
    const std::vector<std::pair<int, int>>& excluded_edges,
    int source,
    int target,
    long long modulus
){
    if(vertex_count <= 0 || source < 0 || vertex_count <= source ||
       target < 0 || vertex_count <= target || modulus <= 0){
        throw std::runtime_error(
            "library assertion fault: range violation "
            "(complement_shortest_path_count)."
        );
    }

    std::vector<std::vector<int>> excluded(
        static_cast<std::size_t>(vertex_count)
    );
    for(const auto [first, second]: excluded_edges){
        if(first < 0 || vertex_count <= first ||
           second < 0 || vertex_count <= second){
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(complement_shortest_path_count)."
            );
        }
        if(first == second) continue;
        excluded[static_cast<std::size_t>(first)].push_back(second);
        excluded[static_cast<std::size_t>(second)].push_back(first);
    }
    for(auto& adjacency: excluded){
        std::sort(adjacency.begin(), adjacency.end());
        adjacency.erase(
            std::unique(adjacency.begin(), adjacency.end()), adjacency.end()
        );
    }

    const auto shortest = complement_shortest_path(excluded, source);
    const int target_distance = shortest.dist[static_cast<std::size_t>(target)];
    if(target_distance == -1) return -1;

    std::vector<std::vector<int>> levels(
        static_cast<std::size_t>(target_distance + 1)
    );
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        const int distance = shortest.dist[static_cast<std::size_t>(vertex)];
        if(0 <= distance && distance <= target_distance){
            levels[static_cast<std::size_t>(distance)].push_back(vertex);
        }
    }

    const auto add_mod = [modulus](long long left, long long right){
        return left >= modulus - right
            ? left - (modulus - right)
            : left + right;
    };
    const auto subtract_mod = [modulus](long long left, long long right){
        return left < right ? left + (modulus - right) : left - right;
    };

    std::vector<long long> ways(static_cast<std::size_t>(vertex_count), 0);
    ways[static_cast<std::size_t>(source)] = 1 % modulus;
    for(int distance = 1; distance <= target_distance; ++distance){
        long long total = 0;
        for(int vertex: levels[static_cast<std::size_t>(distance - 1)]){
            total = add_mod(total, ways[static_cast<std::size_t>(vertex)]);
        }
        for(int vertex: levels[static_cast<std::size_t>(distance)]){
            long long count = total;
            for(int previous: excluded[static_cast<std::size_t>(vertex)]){
                if(shortest.dist[static_cast<std::size_t>(previous)] ==
                   distance - 1){
                    count = subtract_mod(
                        count, ways[static_cast<std::size_t>(previous)]
                    );
                }
            }
            ways[static_cast<std::size_t>(vertex)] = count;
        }
    }
    return ways[static_cast<std::size_t>(target)];
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_SHORTEST_PATH_COMPLEMENT_SHORTEST_PATH_COUNT_HPP_INCLUDED
