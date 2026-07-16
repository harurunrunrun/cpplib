// competitive-verifier: STANDALONE

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/spanning_tree_budget_feasible.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int case_count;
    if(!(std::cin >> case_count)) return 0;
    while(case_count-- > 0){
        int vertex_count, edge_count;
        long long budget;
        std::cin >> vertex_count >> edge_count >> budget;
        std::vector<KruskalEdge<long long>> edges;
        edges.reserve(static_cast<std::size_t>(edge_count));
        while(edge_count-- > 0){
            int from, to;
            long long cost;
            std::cin >> from >> to >> cost;
            edges.push_back({from, to, cost});
        }
        std::cout << (spanning_tree_budget_feasible(
            vertex_count, edges, budget
        ) ? "YES\n" : "NO\n");
    }
}
