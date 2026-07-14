#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>

#include "iterative_strongly_connected_components.hpp"

inline std::vector<int> bottom_vertices(
    const std::vector<std::vector<int>>& graph
){
    const auto components = iterative_strongly_connected_components(graph);
    std::vector<unsigned char> has_outgoing(
        static_cast<std::size_t>(components.count)
    );
    for(int vertex = 0; vertex < static_cast<int>(graph.size()); ++vertex){
        for(const int to: graph[static_cast<std::size_t>(vertex)]){
            const int from_component =
                components.id[static_cast<std::size_t>(vertex)];
            if(from_component != components.id[static_cast<std::size_t>(to)]){
                has_outgoing[static_cast<std::size_t>(from_component)] = 1;
            }
        }
    }
    std::vector<int> answer;
    for(int vertex = 0; vertex < static_cast<int>(graph.size()); ++vertex){
        if(has_outgoing[static_cast<std::size_t>(
            components.id[static_cast<std::size_t>(vertex)]
        )] == 0){
            answer.push_back(vertex);
        }
    }
    return answer;
}
