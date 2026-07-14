#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>

#include "iterative_strongly_connected_components.hpp"

inline std::vector<int> capital_candidate_vertices(
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
    int sink = -1;
    for(int component = 0; component < components.count; ++component){
        if(has_outgoing[static_cast<std::size_t>(component)] != 0) continue;
        if(sink != -1) return {};
        sink = component;
    }
    if(sink == -1) return {};
    std::vector<int> answer = components.groups[static_cast<std::size_t>(sink)];
    std::sort(answer.begin(), answer.end());
    return answer;
}
