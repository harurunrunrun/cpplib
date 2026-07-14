// competitive-verifier: PROBLEM https://atcoder.jp/contests/abc340/tasks/abc340_d

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/minimum_forward_or_jump_cost.hpp"

int main(){
    int n;
    std::cin >> n;
    std::vector<long long> forward_cost(static_cast<std::size_t>(n - 1));
    std::vector<long long> jump_cost(static_cast<std::size_t>(n - 1));
    std::vector<int> jump_target(static_cast<std::size_t>(n - 1));
    for(int v = 0; v + 1 < n; ++v){
        const std::size_t index = static_cast<std::size_t>(v);
        std::cin >> forward_cost[index]
                 >> jump_cost[index]
                 >> jump_target[index];
        --jump_target[index];
    }
    std::cout << minimum_forward_or_jump_cost(
        forward_cost, jump_cost, jump_target
    ) << '\n';
}
