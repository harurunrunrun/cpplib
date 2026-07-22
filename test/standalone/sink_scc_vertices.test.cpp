// competitive-verifier: STANDALONE

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/connectivity/sink_scc_vertices.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count, edge_count;
    if(!(std::cin >> vertex_count >> edge_count)) return 0;
    std::vector<std::vector<int>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    while(edge_count-- != 0){
        int from, to;
        std::cin >> from >> to;
        graph[static_cast<std::size_t>(from)].push_back(to);
    }
    const auto vertices = sink_scc_vertices(graph);
    std::cout << vertices.size() << '\n';
    for(std::size_t index = 0; index < vertices.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << vertices[index];
    }
    std::cout << '\n';
}
