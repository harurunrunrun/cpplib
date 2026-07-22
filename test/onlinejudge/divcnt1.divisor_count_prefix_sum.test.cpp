// competitive-verifier: PROBLEM https://www.spoj.com/problems/DIVCNT1/
#include <cstdint>
#include <iostream>

#include "../../src/algorithm/math/number_theory/divisor_count_prefix_sum.hpp"

namespace{

void print_uint128(math::divisor_count_prefix_sum_value value){
    if(value >= 10) print_uint128(value / 10);
    std::cout.put(static_cast<char>('0' + value % 10));
}

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count--){
        std::uint64_t n;
        std::cin >> n;
        print_uint128(math::divisor_count_prefix_sum(n));
        std::cout << '\n';
    }
}
