// competitive-verifier: PROBLEM https://www.spoj.com/problems/CHICAGO/

#include <iomanip>
#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/maximum_probability_path.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count;
    while(std::cin >> vertex_count && vertex_count != 0){
        int edge_count;
        std::cin >> edge_count;
        std::vector<std::vector<MaximumProbabilityPathEdge<double>>> graph(
            static_cast<std::size_t>(vertex_count)
        );
        while(edge_count-- > 0){
            int left, right, percent;
            std::cin >> left >> right >> percent;
            --left;
            --right;
            const double probability = static_cast<double>(percent) / 100.0;
            graph[static_cast<std::size_t>(left)].push_back({right, probability});
            graph[static_cast<std::size_t>(right)].push_back({left, probability});
        }
        const auto result = maximum_probability_path(graph, 0);
        std::cout << std::fixed << std::setprecision(6)
                  << result.probability[static_cast<std::size_t>(vertex_count - 1)] * 100.0
                  << " percent\n";
    }
}
