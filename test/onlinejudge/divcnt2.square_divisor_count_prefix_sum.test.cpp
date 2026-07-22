// competitive-verifier: PROBLEM https://www.spoj.com/problems/DIVCNT2/
#include <cstdint>
#include <iostream>
#include <vector>

#include "../../src/algorithm/math/number_theory/square_divisor_count_prefix_sum.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    std::vector<std::uint64_t> queries(static_cast<std::size_t>(test_count));
    for(auto& n: queries) std::cin >> n;
    for(const auto answer: math::square_divisor_count_prefix_sums(queries)){
        std::cout << answer << '\n';
    }
}
