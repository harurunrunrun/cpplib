// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/NTL_1_C

#include <iostream>
#include <vector>

#include "../../src/algorithm/math/range_lcm.hpp"

int main(){
    int count;
    std::cin >> count;
    std::vector<long long> values(static_cast<std::size_t>(count));
    for(auto& value: values) std::cin >> value;
    std::cout << math::range_lcm(values) << '\n';
}
