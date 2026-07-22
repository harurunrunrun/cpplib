// competitive-verifier: STANDALONE

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/flow_cut/maximum_paths_with_distinct_terminal_edges.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int case_count;
    if(!(std::cin >> case_count)) return 0;
    while(case_count-- > 0){
        int vertex_count, edge_count, source, sink;
        std::cin >> vertex_count >> edge_count >> source >> sink;
        std::vector<std::pair<int, int>> edges;
        edges.reserve(static_cast<std::size_t>(edge_count));
        while(edge_count-- > 0){
            int from, to;
            std::cin >> from >> to;
            edges.emplace_back(from, to);
        }
        std::cout << maximum_paths_with_distinct_terminal_edges(
            vertex_count, edges, source, sink
        ) << '\n';
    }
}
