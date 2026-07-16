// competitive-verifier: PROBLEM https://www.spoj.com/problems/EC_P/

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/sorted_bridge_endpoints.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int test_count;
    std::cin >> test_count;
    for(int test = 1; test <= test_count; ++test){
        int size, edge_count;
        std::cin >> size >> edge_count;
        std::vector<std::pair<int, int>> edges;
        edges.reserve(static_cast<std::size_t>(edge_count));
        while(edge_count-- != 0){
            int left, right;
            std::cin >> left >> right;
            edges.emplace_back(left - 1, right - 1);
        }
        const auto answer = sorted_bridge_endpoints(size, edges);
        std::cout << "Caso #" << test << '\n';
        if(answer.empty()){
            std::cout << "Sin bloqueos\n";
        }else{
            std::cout << answer.size() << '\n';
            for(const auto [left, right]: answer){
                std::cout << left + 1 << ' ' << right + 1 << '\n';
            }
        }
    }
}
