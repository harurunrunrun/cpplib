// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/specialized/route_optimization/directed_chinese_postman.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int case_count;
    if(!(std::cin >> case_count)) return 1;
    while(case_count-- > 0){
        int vertex_count;
        int edge_count;
        std::cin >> vertex_count >> edge_count;
        std::vector<DirectedChinesePostmanEdge> edges(
            static_cast<std::size_t>(edge_count)
        );
        std::vector<long long> balance(
            static_cast<std::size_t>(vertex_count), 0
        );
        long long base_cost = 0;
        for(auto& edge: edges){
            std::cin >> edge.from >> edge.to >> edge.cost;
            ++balance[static_cast<std::size_t>(edge.from)];
            --balance[static_cast<std::size_t>(edge.to)];
            base_cost += edge.cost;
        }
        auto result = directed_chinese_postman(vertex_count, edges);
        if(!result){
            std::cout << -1 << '\n';
            continue;
        }
        long long reconstructed = base_cost;
        for(const auto& augmentation: result->augmentations){
            balance[static_cast<std::size_t>(augmentation.from)] +=
                augmentation.multiplicity;
            balance[static_cast<std::size_t>(augmentation.to)] -=
                augmentation.multiplicity;
            reconstructed +=
                augmentation.multiplicity * augmentation.distance;
        }
        for(long long value: balance) if(value != 0) return 2;
        if(reconstructed != result->cost) return 3;
        std::cout << result->cost << '\n';
    }
}
