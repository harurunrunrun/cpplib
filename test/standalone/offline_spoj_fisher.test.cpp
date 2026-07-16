// competitive-verifier: STANDALONE

#define main onlinejudge_adapter_main
#include "../onlinejudge/fisher.minimum_cost_path_with_resource_limit.test.cpp"
#undef main

#include <cassert>
#include <limits>
#include <stdexcept>

int main(){
    std::vector<std::vector<ResourceConstrainedEdge>> graph(4);
    graph[2].push_back({0, 2, 5});
    graph[0].push_back({3, 1, 7});
    graph[2].push_back({3, 4, 20});

    const auto answer = minimum_cost_path_with_resource_limit(graph, 2, 3, 3);
    assert(answer && answer->cost == 12 && answer->resource_used == 3);
    assert(!minimum_cost_path_with_resource_limit(graph, 2, 3, 2));

    bool invalid_edge_rejected = false;
    try{
        auto invalid_graph = graph;
        invalid_graph[1].push_back({2, 0, 1});
        (void)minimum_cost_path_with_resource_limit(invalid_graph, 0, 3, 3);
    }catch(const std::runtime_error&){
        invalid_edge_rejected = true;
    }
    assert(invalid_edge_rejected);

    bool overflow_rejected = false;
    try{
        std::vector<std::vector<ResourceConstrainedEdge>> overflow_graph(3);
        overflow_graph[0].push_back({
            1,
            1,
            std::numeric_limits<long long>::max()
        });
        overflow_graph[1].push_back({
            2,
            1,
            std::numeric_limits<long long>::max()
        });
        (void)minimum_cost_path_with_resource_limit(
            overflow_graph,
            0,
            2,
            2
        );
    }catch(const std::overflow_error&){
        overflow_rejected = true;
    }
    assert(overflow_rejected);
    return onlinejudge_adapter_main();
}
