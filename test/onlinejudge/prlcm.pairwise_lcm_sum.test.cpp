// competitive-verifier: PROBLEM https://www.spoj.com/problems/PRLCM/
#include <iostream>
#include <vector>

#include "../../src/algorithm/math/pairwise_lcm_sum.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int size;
    std::cin >> size;
    std::vector<int> values(static_cast<std::size_t>(size));
    for(int& value: values) std::cin >> value;
    std::cout << math::pairwise_lcm_sum(values) << '\n';
}
