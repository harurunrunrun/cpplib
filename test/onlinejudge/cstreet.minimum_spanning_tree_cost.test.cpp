// competitive-verifier: PROBLEM https://www.spoj.com/problems/CSTREET/

#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/graph/spanning_tree/minimum_spanning_tree_cost.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count-- > 0){
        long long unit_price;
        int vertex_count, edge_count;
        std::cin >> unit_price >> vertex_count >> edge_count;
        std::vector<KruskalEdge<long long>> edges;
        edges.reserve(static_cast<std::size_t>(edge_count));
        while(edge_count-- > 0){
            int from, to;
            long long length;
            std::cin >> from >> to >> length;
            edges.push_back({from - 1, to - 1, length});
        }
        const __int128_t answer = static_cast<__int128_t>(unit_price) *
            minimum_spanning_tree_cost(vertex_count, edges);
        if(answer < std::numeric_limits<long long>::min() ||
           answer > std::numeric_limits<long long>::max())[[unlikely]]{
            throw std::overflow_error("CSTREET answer overflow");
        }
        std::cout << static_cast<long long>(answer) << '\n';
    }
}
