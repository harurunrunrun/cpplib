// competitive-verifier: STANDALONE

#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/graph/strongly_connected_components.hpp"

int main(){
    int vertex_count, edge_count, query_count;
    std::cin >> vertex_count >> edge_count >> query_count;
    std::vector<std::vector<int>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    while(edge_count--){
        int from, to;
        std::cin >> from >> to;
        graph[static_cast<std::size_t>(from)].push_back(to);
    }
    const auto result = strongly_connected_components(graph);
    while(query_count--){
        int first, second;
        std::cin >> first >> second;
        try{
            std::cout << result.same_component(first, second) << '\n';
        }catch(const std::runtime_error&){
            std::cout << "ERR\n";
        }
    }
}
