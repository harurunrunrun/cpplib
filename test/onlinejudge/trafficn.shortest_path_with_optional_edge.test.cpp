// competitive-verifier: PROBLEM https://www.spoj.com/problems/TRAFFICN/

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/shortest_path_with_optional_edge.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count-- > 0){
        int vertex_count, edge_count, candidate_count, source, target;
        std::cin >> vertex_count >> edge_count >> candidate_count >> source >> target;
        --source;
        --target;

        std::vector<std::vector<DijkstraEdge<long long>>> graph(
            static_cast<std::size_t>(vertex_count)
        );
        for(int index = 0; index < edge_count; ++index){
            int from, to;
            long long cost;
            std::cin >> from >> to >> cost;
            graph[static_cast<std::size_t>(from - 1)].push_back({to - 1, cost});
        }

        std::vector<OptionalShortestPathEdge<long long>> candidates;
        candidates.reserve(static_cast<std::size_t>(candidate_count));
        for(int index = 0; index < candidate_count; ++index){
            int from, to;
            long long cost;
            std::cin >> from >> to >> cost;
            candidates.push_back({from - 1, to - 1, cost});
        }

        const auto answer = shortest_path_with_at_most_one_optional_edge(
            graph, source, target, candidates, true
        );
        std::cout << (answer ? *answer : -1) << '\n';
    }
}
