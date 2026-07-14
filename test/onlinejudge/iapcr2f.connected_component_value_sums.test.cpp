// competitive-verifier: PROBLEM https://www.spoj.com/problems/IAPCR2F/

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/connected_component_value_sums.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int test_count;
    std::cin >> test_count;
    for(int test = 1; test <= test_count; ++test){
        int size, edge_count;
        std::cin >> size >> edge_count;
        std::vector<long long> values(static_cast<std::size_t>(size));
        for(long long& value: values) std::cin >> value;
        std::vector<std::pair<int, int>> edges;
        edges.reserve(static_cast<std::size_t>(edge_count));
        while(edge_count-- != 0){
            int left, right;
            std::cin >> left >> right;
            edges.emplace_back(left - 1, right - 1);
        }
        const auto sums = connected_component_value_sums(values, edges);
        std::cout << "Case " << test << ": " << sums.size() << '\n';
        for(std::size_t index = 0; index < sums.size(); ++index){
            if(index != 0) std::cout << ' ';
            std::cout << sums[index];
        }
        std::cout << '\n';
    }
}
