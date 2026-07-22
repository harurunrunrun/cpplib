// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/shortest_path/minimum_forward_or_jump_cost.hpp"

void self_test(){
    assert(minimum_forward_or_jump_cost(
        std::vector<long long>{},
        std::vector<long long>{},
        std::vector<int>{}
    ) == 0);
    assert(minimum_forward_or_jump_cost(
        std::vector<long long>{5},
        std::vector<long long>{2},
        std::vector<int>{1}
    ) == 2);
    assert(minimum_forward_or_jump_cost(
        std::vector<long long>{10, 10, 10},
        std::vector<long long>{1, 1, 1},
        std::vector<int>{0, 2, 3}
    ) == 12);
}

int main(){
    self_test();

    int n;
    if(!(std::cin >> n)) return 0;
    std::vector<long long> forward_cost(static_cast<std::size_t>(n - 1));
    std::vector<long long> jump_cost(static_cast<std::size_t>(n - 1));
    std::vector<int> jump_target(static_cast<std::size_t>(n - 1));
    for(int vertex = 0; vertex + 1 < n; ++vertex){
        const std::size_t index = static_cast<std::size_t>(vertex);
        std::cin >> forward_cost[index]
                 >> jump_cost[index]
                 >> jump_target[index];
        --jump_target[index];
    }
    std::cout << minimum_forward_or_jump_cost(
        forward_cost, jump_cost, jump_target
    ) << '\n';
}
