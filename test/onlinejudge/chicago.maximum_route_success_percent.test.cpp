// competitive-verifier: PROBLEM https://www.spoj.com/problems/CHICAGO/

#include <iomanip>
#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/maximum_route_success_percent.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count;
    while(std::cin >> vertex_count && vertex_count != 0){
        int edge_count;
        std::cin >> edge_count;
        std::vector<RouteSuccessPercentEdge> edges;
        edges.reserve(static_cast<std::size_t>(edge_count));
        while(edge_count-- > 0){
            int left, right, percent;
            std::cin >> left >> right >> percent;
            edges.push_back({left - 1, right - 1, percent});
        }
        std::cout << std::fixed << std::setprecision(6)
                  << maximum_route_success_percent(vertex_count, edges)
                  << " percent\n";
    }
}
