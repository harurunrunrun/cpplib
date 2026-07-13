#pragma once

#include <queue>
#include <stdexcept>
#include <vector>

struct ComplementConnectedComponentsResult{
    std::vector<int> component_id;
    std::vector<std::vector<int>> groups;
};

inline ComplementConnectedComponentsResult connected_components_of_complement_graph(
    const std::vector<std::vector<int>>& graph
){
    const int n = static_cast<int>(graph.size());
    for(const auto& edges: graph){
        for(int to: edges){
            if(to < 0 || n <= to)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: range violation "
                    "(connected_components_of_complement_graph)."
                );
            }
        }
    }

    ComplementConnectedComponentsResult result;
    result.component_id.assign(static_cast<std::size_t>(n), -1);
    std::vector<int> remaining;
    remaining.reserve(static_cast<std::size_t>(n));
    for(int vertex = 0; vertex < n; ++vertex) remaining.push_back(vertex);

    std::vector<int> marked(static_cast<std::size_t>(n), 0);
    int stamp = 0;
    std::queue<int> queue;
    while(!remaining.empty()){
        const int start = remaining.back();
        remaining.pop_back();
        const int component = static_cast<int>(result.groups.size());
        result.groups.push_back({start});
        result.component_id[static_cast<std::size_t>(start)] = component;
        queue.push(start);

        while(!queue.empty()){
            const int vertex = queue.front();
            queue.pop();
            ++stamp;
            for(int to: graph[static_cast<std::size_t>(vertex)]){
                marked[static_cast<std::size_t>(to)] = stamp;
            }

            std::vector<int> next_remaining;
            next_remaining.reserve(remaining.size());
            for(int candidate: remaining){
                if(marked[static_cast<std::size_t>(candidate)] == stamp){
                    next_remaining.push_back(candidate);
                }else{
                    result.component_id[static_cast<std::size_t>(candidate)] = component;
                    result.groups.back().push_back(candidate);
                    queue.push(candidate);
                }
            }
            remaining.swap(next_remaining);
        }
    }
    return result;
}
