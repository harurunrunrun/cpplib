// competitive-verifier: PROBLEM https://www.spoj.com/problems/METEORS/

#include <iostream>
#include <vector>

#include "../../src/algorithm/other/offline_query/earliest_circular_range_requirement_times.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int owner_count, position_count;
    std::cin >> owner_count >> position_count;
    std::vector<std::vector<int>> owned_positions(
        static_cast<std::size_t>(owner_count)
    );
    for(int position = 0; position < position_count; ++position){
        int owner;
        std::cin >> owner;
        owned_positions[static_cast<std::size_t>(owner - 1)].push_back(position);
    }
    std::vector<long long> requirements(static_cast<std::size_t>(owner_count));
    for(long long& requirement: requirements) std::cin >> requirement;

    int operation_count;
    std::cin >> operation_count;
    std::vector<CircularRangeAddition> operations(
        static_cast<std::size_t>(operation_count)
    );
    for(auto& operation: operations){
        std::cin >> operation.left >> operation.right >> operation.amount;
        --operation.left;
        --operation.right;
    }

    for(int answer: earliest_circular_range_requirement_times(
        position_count, owned_positions, requirements, operations
    )){
        if(answer == operation_count + 1) std::cout << "NIE\n";
        else std::cout << answer << '\n';
    }
}
