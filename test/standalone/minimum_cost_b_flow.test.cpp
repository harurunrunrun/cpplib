// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/matching/flow/minimum_cost_b_flow.hpp"
#include "../../src/algorithm/matching/flow/minimum_cost_circulation.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int case_count;
    if(!(std::cin >> case_count)) return 1;
    while(case_count-- > 0){
        int kind;
        int vertex_count;
        int edge_count;
        std::cin >> kind >> vertex_count >> edge_count;
        std::vector<long long> balance(
            static_cast<std::size_t>(vertex_count), 0
        );
        if(kind == 1){
            for(long long& value: balance) std::cin >> value;
        }
        std::vector<MinimumCostBFlowEdge> edges(
            static_cast<std::size_t>(edge_count)
        );
        for(auto& edge: edges){
            std::cin >> edge.from >> edge.to
                     >> edge.lower >> edge.upper >> edge.cost;
        }
        std::optional<MinimumCostBFlowResult> result;
        if(kind == 0){
            result = minimum_cost_circulation(vertex_count, edges);
        }else{
            MinimumCostBFlow solver(vertex_count);
            for(int vertex = 0; vertex < vertex_count; ++vertex){
                solver.set_balance(
                    vertex, balance[static_cast<std::size_t>(vertex)]
                );
            }
            for(const auto& edge: edges){
                solver.add_edge(
                    edge.from, edge.to,
                    edge.lower, edge.upper, edge.cost
                );
            }
            result = solver.solve();
        }
        if(!result){
            std::cout << "none\n";
            continue;
        }
        std::vector<__int128> actual_balance(
            static_cast<std::size_t>(vertex_count), 0
        );
        __int128 actual_cost = 0;
        for(std::size_t index = 0; index < edges.size(); ++index){
            const long long value = result->flow[index];
            if(value < edges[index].lower || edges[index].upper < value){
                return 2;
            }
            actual_balance[static_cast<std::size_t>(edges[index].from)]
                -= value;
            actual_balance[static_cast<std::size_t>(edges[index].to)]
                += value;
            actual_cost += static_cast<__int128>(value) * edges[index].cost;
        }
        for(int vertex = 0; vertex < vertex_count; ++vertex){
            if(actual_balance[static_cast<std::size_t>(vertex)]
                != balance[static_cast<std::size_t>(vertex)]) return 3;
        }
        if(actual_cost != result->cost) return 4;
        std::cout << static_cast<long long>(result->cost) << '\n';
    }
}
