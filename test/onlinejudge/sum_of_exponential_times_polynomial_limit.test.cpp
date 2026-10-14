// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/sum_of_exponential_times_polynomial_limit
#include <iostream>
#include "../../src/algorithm/math/sequence/exponential_power_sum.hpp"
int main(){
    Modint998244353 r; std::size_t d;
    std::cin >> r >> d;
    std::cout << math::exponential_power_sum_limit<998244353>(r,d) << '\n';
}
