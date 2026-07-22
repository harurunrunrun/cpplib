// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/graph/spanning_tree/count_minimum_spanning_trees.hpp"

using Edge = MinimumSpanningTreeCountEdge<long long>;
constexpr std::uint64_t modulus = 1'000'000'007ULL;

void run_direct_tests(){
    {
        const auto result =
            count_minimum_spanning_trees<modulus, long long>(0, {});
        assert(result.connected);
        assert(result.minimum_weight == 0);
        assert(result.count_mod == 1);
    }
    {
        const std::vector<Edge> edges{{0, 0, -100}, {0, 0, 5}};
        const auto result = count_minimum_spanning_trees<modulus>(1, edges);
        assert(result.connected);
        assert(result.minimum_weight == 0);
        assert(result.count_mod == 1);
    }
    {
        const std::vector<Edge> edges{
            {0, 1, 2}, {0, 1, 2}, {1, 2, -1}, {0, 2, 5}, {2, 2, -7}
        };
        const auto result = count_minimum_spanning_trees<modulus>(3, edges);
        assert(result.connected);
        assert(result.minimum_weight == 1);
        assert(result.count_mod == 2);
    }
    {
        std::vector<Edge> complete_graph;
        for(int first = 0; first < 4; ++first){
            for(int second = first + 1; second < 4; ++second){
                complete_graph.push_back({first, second, 0});
            }
        }
        const auto result =
            count_minimum_spanning_trees<6>(4, complete_graph);
        assert(result.connected);
        assert(result.minimum_weight == 0);
        assert(result.count_mod == 4); // 4^(4-2) modulo 6
    }
    {
        const std::vector<Edge> edges{{0, 1, 1}, {2, 3, 1}};
        const auto result = count_minimum_spanning_trees<modulus>(4, edges);
        assert(!result.connected);
        assert(result.minimum_weight == 0);
        assert(result.count_mod == 0);
    }
    {
        bool endpoint_threw = false;
        try{
            (void) count_minimum_spanning_trees<modulus>(
                2,
                std::vector<Edge>{{0, 2, 1}}
            );
        }catch(const std::out_of_range&){
            endpoint_threw = true;
        }
        assert(endpoint_threw);
    }
}

int main(){
    int case_count;
    if(!(std::cin >> case_count)){
        run_direct_tests();
        return 0;
    }
    while(case_count--){
        int vertex_count, edge_count;
        std::cin >> vertex_count >> edge_count;
        std::vector<Edge> edges(edge_count);
        for(auto& edge: edges){
            std::cin >> edge.from >> edge.to >> edge.weight;
        }
        const auto result =
            count_minimum_spanning_trees<modulus>(vertex_count, edges);
        std::cout << result.connected << ' '
                  << result.minimum_weight << ' '
                  << result.count_mod << '\n';
    }
}
