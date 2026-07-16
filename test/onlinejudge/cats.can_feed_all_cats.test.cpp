// competitive-verifier: PROBLEM https://open.kattis.com/problems/cats

#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/spanning_tree_budget_feasible.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count--){
        long long milk;
        int cat_count;
        std::cin >> milk >> cat_count;
        const int edge_count = cat_count * (cat_count - 1) / 2;
        std::vector<KruskalEdge<long long>> edges;
        edges.reserve(static_cast<std::size_t>(edge_count));
        for(int index = 0; index < edge_count; ++index){
            int first, second;
            long long distance;
            std::cin >> first >> second >> distance;
            edges.push_back({first, second, distance});
        }
        const bool feasible =
            milk >= cat_count &&
            spanning_tree_budget_feasible(
                cat_count, edges, milk - cat_count
            );
        std::cout << (feasible ? "yes\n" : "no\n");
    }
}
