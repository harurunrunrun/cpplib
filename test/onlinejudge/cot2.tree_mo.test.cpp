// competitive-verifier: PROBLEM https://www.spoj.com/problems/COT2/

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/tree/tree_mo.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count, query_count;
    std::cin >> vertex_count >> query_count;

    std::vector<int> values(static_cast<std::size_t>(vertex_count));
    for(int& value : values) std::cin >> value;
    std::vector<int> sorted_values = values;
    std::sort(sorted_values.begin(), sorted_values.end());
    sorted_values.erase(
        std::unique(sorted_values.begin(), sorted_values.end()),
        sorted_values.end()
    );
    for(int& value : values){
        value = static_cast<int>(
            std::lower_bound(
                sorted_values.begin(),
                sorted_values.end(),
                value
            ) - sorted_values.begin()
        );
    }

    TreeMo<40000> mo(vertex_count);
    for(int edge = 1; edge < vertex_count; ++edge){
        int left, right;
        std::cin >> left >> right;
        mo.add_edge(left - 1, right - 1);
    }
    mo.build(0);
    mo.reserve_queries(query_count);
    for(int query = 0; query < query_count; ++query){
        int left, right;
        std::cin >> left >> right;
        mo.add_query(left - 1, right - 1);
    }

    std::vector<int> frequency(sorted_values.size(), 0);
    std::vector<int> answer(static_cast<std::size_t>(query_count), 0);
    int distinct = 0;
    mo.solve(
        [&](int vertex){
            const int value = values[static_cast<std::size_t>(vertex)];
            if(frequency[static_cast<std::size_t>(value)]++ == 0){
                ++distinct;
            }
        },
        [&](int vertex){
            const int value = values[static_cast<std::size_t>(vertex)];
            if(--frequency[static_cast<std::size_t>(value)] == 0){
                --distinct;
            }
        },
        [&](int query){
            answer[static_cast<std::size_t>(query)] = distinct;
        }
    );

    for(const int value : answer) std::cout << value << '\n';
}
