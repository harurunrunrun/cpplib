#pragma once

#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

inline bool is_bipartite(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
){
    if(vertex_count < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation "
            "(is_bipartite)."
        );
    }
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(vertex_count));
    for(const auto& [left, right]: edges){
        if(left < 0 || vertex_count <= left || right < 0 || vertex_count <= right)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(is_bipartite)."
            );
        }
        graph[static_cast<std::size_t>(left)].push_back(right);
        graph[static_cast<std::size_t>(right)].push_back(left);
    }
    std::vector<signed char> color(static_cast<std::size_t>(vertex_count), -1);
    for(int start = 0; start < vertex_count; start++){
        if(color[static_cast<std::size_t>(start)] != -1) continue;
        color[static_cast<std::size_t>(start)] = 0;
        std::queue<int> queue;
        queue.push(start);
        while(!queue.empty()){
            const int vertex = queue.front();
            queue.pop();
            for(int to: graph[static_cast<std::size_t>(vertex)]){
                if(color[static_cast<std::size_t>(to)] == -1){
                    color[static_cast<std::size_t>(to)] =
                        static_cast<signed char>(color[static_cast<std::size_t>(vertex)] ^ 1);
                    queue.push(to);
                }else if(color[static_cast<std::size_t>(to)] ==
                         color[static_cast<std::size_t>(vertex)]){
                    return false;
                }
            }
        }
    }
    return true;
}
