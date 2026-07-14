// competitive-verifier: PROBLEM https://www.spoj.com/problems/RACETIME/

#include <iostream>
#include <vector>

#include "../../src/algorithm/other/dynamic_point_add_rectangle_sum.hpp"

struct RacetimeOperation{
    char type;
    int first;
    int second;
    long long value;
};

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, query_count;
    std::cin >> n >> query_count;

    std::vector<long long> values(static_cast<std::size_t>(n));
    DynamicPointAddRectangleSum<int> structure;
    for(int i = 0; i < n; i++){
        std::cin >> values[static_cast<std::size_t>(i)];
        structure.reserve_point(i + 1, values[static_cast<std::size_t>(i)]);
    }

    std::vector<RacetimeOperation> operations;
    operations.reserve(static_cast<std::size_t>(query_count));
    for(int query = 0; query < query_count; query++){
        char type;
        std::cin >> type;
        if(type == 'M'){
            int index;
            long long value;
            std::cin >> index >> value;
            operations.push_back({type, index, 0, value});
            structure.reserve_point(index, value);
        }else{
            int left, right;
            long long threshold;
            std::cin >> left >> right >> threshold;
            operations.push_back({type, left, right, threshold});
        }
    }

    structure.build();
    for(int i = 0; i < n; i++){
        structure.add(i + 1, values[static_cast<std::size_t>(i)], 1);
    }

    for(const auto& operation: operations){
        if(operation.type == 'M'){
            const std::size_t index = static_cast<std::size_t>(operation.first - 1);
            structure.add(operation.first, values[index], -1);
            values[index] = operation.value;
            structure.add(operation.first, values[index], 1);
        }else{
            std::cout
                << structure.prefix_sum_leq(operation.second, operation.value)
                    - structure.prefix_sum_leq(operation.first - 1, operation.value)
                << '\n';
        }
    }
}
