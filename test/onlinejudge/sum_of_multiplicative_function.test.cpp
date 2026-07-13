// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/sum_of_multiplicative_function

#include <cstdint>
#include <iostream>

#include "../../src/algorithm/math/sum_of_multiplicative_function_large.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    constexpr int MOD = 469762049;
    using mint = Modint<MOD>;

    int test_count;
    std::cin >> test_count;
    while(test_count-- != 0){
        std::uint64_t n;
        mint a, b;
        std::cin >> n >> a >> b;
        std::cout << math::sum_of_multiplicative_function_large(n, a, b)
                  << '\n';
    }
}
