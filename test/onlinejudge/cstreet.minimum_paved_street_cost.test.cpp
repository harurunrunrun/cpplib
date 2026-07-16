// competitive-verifier: PROBLEM https://www.spoj.com/problems/CSTREET/

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/minimum_paved_street_cost.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count-- > 0){
        long long price;
        int vertex_count, edge_count;
        std::cin >> price >> vertex_count >> edge_count;
        std::vector<KruskalEdge<long long>> edges;
        edges.reserve(static_cast<std::size_t>(edge_count));
        while(edge_count-- > 0){
            int from, to;
            long long length;
            std::cin >> from >> to >> length;
            edges.push_back({from - 1, to - 1, length});
        }
        std::cout << minimum_paved_street_cost(price, vertex_count, edges)
                  << '\n';
    }
}
