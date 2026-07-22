// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/graph/shortest_path/shortest_path_with_optional_edge.hpp"

namespace{

template<class Function>
void expect_shortest_path_error(Function&& function){
    bool thrown = false;
    try{
        function();
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

void self_test(){
    using Edge = DijkstraEdge<long long>;
    using OptionalEdge = OptionalShortestPathEdge<long long>;

    std::vector<std::vector<Edge>> graph(4);
    graph[0].push_back({2, 3});
    graph[1].push_back({3, 4});
    const std::vector<OptionalEdge> candidates = {{1, 2, 2}};
    assert(!shortest_path_with_at_most_one_optional_edge(
        graph, 0, 3, candidates, false
    ));
    assert(shortest_path_with_at_most_one_optional_edge(
        graph, 0, 3, candidates, true
    ) == 9);

    graph[0].push_back({3, 20});
    assert(shortest_path_with_at_most_one_optional_edge(
        graph, 0, 3, {}, false
    ) == 20);

    expect_shortest_path_error([&]{
        (void)shortest_path_with_at_most_one_optional_edge(
            graph, -1, 3, candidates
        );
    });
    expect_shortest_path_error([&]{
        (void)shortest_path_with_at_most_one_optional_edge(
            graph, 0, 4, candidates
        );
    });
    expect_shortest_path_error([&]{
        (void)shortest_path_with_at_most_one_optional_edge(
            graph, 0, 3, std::vector<OptionalEdge>{{0, 4, 1}}
        );
    });
    expect_shortest_path_error([&]{
        (void)shortest_path_with_at_most_one_optional_edge(
            graph, 0, 3, std::vector<OptionalEdge>{{0, 1, -1}}
        );
    });
}

}  // namespace

int main(){
    self_test();
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int query_count;
    if(!(std::cin >> query_count)) return 0;
    while(query_count-- > 0){
        int vertex_count, edge_count, candidate_count;
        int source, target, bidirectional;
        std::cin >> vertex_count >> edge_count >> candidate_count
                 >> source >> target >> bidirectional;
        std::vector<std::vector<DijkstraEdge<long long>>> graph(
            static_cast<std::size_t>(vertex_count)
        );
        for(int index = 0; index < edge_count; ++index){
            int from, to;
            long long cost;
            std::cin >> from >> to >> cost;
            graph[static_cast<std::size_t>(from)].push_back({to, cost});
        }
        std::vector<OptionalShortestPathEdge<long long>> candidates(
            static_cast<std::size_t>(candidate_count)
        );
        for(auto& edge: candidates){
            std::cin >> edge.from >> edge.to >> edge.cost;
        }
        const auto answer = shortest_path_with_at_most_one_optional_edge(
            graph, source, target, candidates, bidirectional != 0
        );
        std::cout << (answer ? *answer : -1) << '\n';
    }
}
