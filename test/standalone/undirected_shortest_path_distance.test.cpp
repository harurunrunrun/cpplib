// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/graph/shortest_path/undirected_shortest_path_distance.hpp"

void self_test(){
    const std::vector<UndirectedShortestPathEdge<int>> edges{
        {0, 1, 5}, {1, 2, 6}, {0, 2, 20}
    };
    assert(undirected_shortest_path_distance(4, edges, 0, 2) == 11);
    assert(!undirected_shortest_path_distance(4, edges, 0, 3));

    bool thrown = false;
    try{
        (void)undirected_shortest_path_distance<int>(
            2, {{0, 1, -1}}, 0, 1
        );
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    self_test();

    int vertex_count, edge_count, source, target;
    if(!(std::cin >> vertex_count >> edge_count >> source >> target)) return 0;
    std::vector<UndirectedShortestPathEdge<long long>> edges;
    edges.reserve(static_cast<std::size_t>(edge_count));
    while(edge_count-- > 0){
        int left, right;
        long long cost;
        std::cin >> left >> right >> cost;
        edges.push_back({left, right, cost});
    }
    const auto answer = undirected_shortest_path_distance(
        vertex_count, edges, source, target
    );
    if(answer) std::cout << *answer << '\n';
    else std::cout << "unreachable\n";
}
