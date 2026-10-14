// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/sum_of_exponential_times_polynomial
#include <cstdint>
#include <iostream>
#include "../../src/algorithm/math/sequence/exponential_power_sum.hpp"
int main(){
    Modint998244353 r; std::size_t d; std::uint64_t n;
    std::cin >> r >> d >> n;
    std::cout << math::exponential_power_sum<998244353>(r,d,n) << '\n';
}
