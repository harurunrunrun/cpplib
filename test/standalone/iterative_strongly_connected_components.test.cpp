// competitive-verifier: STANDALONE

#include <algorithm>
#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/iterative_strongly_connected_components.hpp"

int main(){
    int vertex_count, edge_count;
    if(!(std::cin >> vertex_count >> edge_count)) return 0;
    std::vector<std::vector<int>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    while(edge_count--){
        int from, to;
        std::cin >> from >> to;
        graph[static_cast<std::size_t>(from)].push_back(to);
    }
    const auto result = iterative_strongly_connected_components(graph);
    std::vector<int> representative(
        static_cast<std::size_t>(vertex_count), vertex_count
    );
    for(const auto& group: result.groups){
        const int minimum = *std::min_element(group.begin(), group.end());
        for(const int vertex: group){
            representative[static_cast<std::size_t>(vertex)] = minimum;
        }
    }
    std::cout << result.count << '\n';
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        if(vertex) std::cout << ' ';
        std::cout << representative[static_cast<std::size_t>(vertex)];
    }
    std::cout << '\n';
}
