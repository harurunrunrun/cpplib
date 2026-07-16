#pragma once

#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

inline int unweighted_tree_radius(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
){
    if(vertex_count < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (unweighted_tree_radius)."
        );
    }
    if(edges.size() != static_cast<std::size_t>(vertex_count == 0 ? 0 : vertex_count - 1))[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: not a tree (unweighted_tree_radius)."
        );
    }
    if(vertex_count == 0) return 0;
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(vertex_count));
    for(const auto& [left, right]: edges){
        if(left < 0 || vertex_count <= left || right < 0 || vertex_count <= right)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (unweighted_tree_radius)."
            );
        }
        graph[static_cast<std::size_t>(left)].push_back(right);
        graph[static_cast<std::size_t>(right)].push_back(left);
    }
    std::vector<unsigned char> seen(static_cast<std::size_t>(vertex_count), 0);
    std::queue<int> connectivity_queue;
    seen[0] = 1;
    connectivity_queue.push(0);
    int reached_count = 0;
    while(!connectivity_queue.empty()){
        const int vertex = connectivity_queue.front();
        connectivity_queue.pop();
        reached_count++;
        for(int to: graph[static_cast<std::size_t>(vertex)]){
            if(seen[static_cast<std::size_t>(to)]) continue;
            seen[static_cast<std::size_t>(to)] = 1;
            connectivity_queue.push(to);
        }
    }
    if(reached_count != vertex_count)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: not a tree (unweighted_tree_radius)."
        );
    }
    auto farthest = [&](int start){
        std::vector<int> distance(static_cast<std::size_t>(vertex_count), -1);
        std::queue<int> queue;
        distance[static_cast<std::size_t>(start)] = 0;
        queue.push(start);
        int result = start;
        while(!queue.empty()){
            const int vertex = queue.front();
            queue.pop();
            if(distance[static_cast<std::size_t>(result)] <
               distance[static_cast<std::size_t>(vertex)]) result = vertex;
            for(int to: graph[static_cast<std::size_t>(vertex)]){
                if(distance[static_cast<std::size_t>(to)] != -1) continue;
                distance[static_cast<std::size_t>(to)] =
                    distance[static_cast<std::size_t>(vertex)] + 1;
                queue.push(to);
            }
        }
        return std::pair<int, int>{
            result, distance[static_cast<std::size_t>(result)]
        };
    };
    const auto endpoint = farthest(0);
    const int diameter = farthest(endpoint.first).second;
    return (diameter + 1) / 2;
}
