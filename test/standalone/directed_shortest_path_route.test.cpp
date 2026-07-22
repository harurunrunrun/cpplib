// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/shortest_path/directed_shortest_path_route.hpp"

namespace{

void self_test(){
    const std::vector<DirectedShortestPathRouteEdge<int>> edges{
        {0, 1, 5}, {1, 2, 6}, {0, 2, 20}
    };
    const auto route = directed_shortest_path_route(3, edges, 0, 2);
    assert(route.reachable);
    assert(route.distance == 11);
    assert((route.edges == std::vector<std::pair<int, int>>{{0, 1}, {1, 2}}));
    assert(!directed_shortest_path_route(4, edges, 0, 3).reachable);

    const auto empty = directed_shortest_path_route(3, edges, 1, 1);
    assert(empty.reachable && empty.distance == 0 && empty.edges.empty());

    bool thrown = false;
    try{
        (void)directed_shortest_path_route<int>(2, {{0, 1, -1}}, 0, 1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

} // namespace

int main(){
    self_test();

    int vertex_count, edge_count, source, target;
    if(!(std::cin >> vertex_count >> edge_count >> source >> target)) return 0;
    std::vector<DirectedShortestPathRouteEdge<long long>> edges;
    edges.reserve(static_cast<std::size_t>(edge_count));
    while(edge_count-- > 0){
        int from, to;
        long long cost;
        std::cin >> from >> to >> cost;
        edges.push_back({from, to, cost});
    }
    const auto answer = directed_shortest_path_route(
        vertex_count, edges, source, target
    );
    if(!answer.reachable){
        std::cout << -1 << '\n';
        return 0;
    }
    std::cout << answer.distance << ' ' << answer.edges.size() << '\n';
    for(const auto& [from, to]: answer.edges){
        std::cout << from << ' ' << to << '\n';
    }
}
