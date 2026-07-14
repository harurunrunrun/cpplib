// competitive-verifier: STANDALONE

#include <cmath>
#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/maximum_probability_path.hpp"

int main(){
    int vertex_count, edge_count, source;
    if(!(std::cin >> vertex_count >> edge_count >> source)) return 0;
    std::vector<std::vector<MaximumProbabilityPathEdge<double>>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    while(edge_count--){
        int from, to, half_units;
        std::cin >> from >> to >> half_units;
        graph[static_cast<std::size_t>(from)].push_back({
            to, static_cast<double>(half_units) / 2.0
        });
    }
    const auto result = maximum_probability_path(graph, source);
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        const std::size_t index = static_cast<std::size_t>(vertex);
        std::cout << static_cast<int>(result.reachable[index]) << ' '
                  << std::llround(result.probability[index] * 1'000'000.0)
                  << '\n';
    }
}
