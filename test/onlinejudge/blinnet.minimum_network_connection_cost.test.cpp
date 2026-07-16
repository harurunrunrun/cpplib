// competitive-verifier: PROBLEM https://www.spoj.com/problems/BLINNET/

#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/graph/minimum_spanning_tree_cost.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count-- > 0){
        int vertex_count;
        std::cin >> vertex_count;
        std::vector<KruskalEdge<long long>> edges;
        for(int vertex = 0; vertex < vertex_count; vertex++){
            std::string city_name;
            int neighbor_count;
            std::cin >> city_name >> neighbor_count;
            while(neighbor_count-- > 0){
                int neighbor;
                long long cost;
                std::cin >> neighbor >> cost;
                edges.push_back({vertex, neighbor - 1, cost});
            }
        }
        std::cout << minimum_spanning_tree_cost(vertex_count, edges)
                  << '\n';
    }
}
