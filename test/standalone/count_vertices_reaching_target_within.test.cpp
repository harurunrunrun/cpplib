// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/graph/shortest_path/count_vertices_reaching_target_within.hpp"

void self_test(){
    const std::vector<ReachingTargetEdge<int>> edges{
        {0, 1, 2}, {1, 2, 3}, {3, 2, 6}
    };
    assert(count_vertices_reaching_target_within(4, edges, 2, 5) == 3);
    assert(count_vertices_reaching_target_within(4, edges, 2, -1) == 0);

    bool thrown = false;
    try{
        (void)count_vertices_reaching_target_within<int>(
            2, {{0, 2, 1}}, 0, 1
        );
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)count_vertices_reaching_target_within<int>(
            2, {{0, 1, -1}}, 1, 1
        );
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    self_test();

    int vertex_count, edge_count, target;
    long long limit;
    if(!(std::cin >> vertex_count >> edge_count >> target >> limit)) return 0;
    std::vector<ReachingTargetEdge<long long>> edges;
    edges.reserve(static_cast<std::size_t>(edge_count));
    while(edge_count-- > 0){
        int from, to;
        long long cost;
        std::cin >> from >> to >> cost;
        edges.push_back({from, to, cost});
    }
    std::cout << count_vertices_reaching_target_within(
                     vertex_count, edges, target, limit
                 )
              << '\n';
}
