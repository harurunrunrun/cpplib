// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/two_square_sum
#include <cstdint>
#include <iostream>
#include "../../src/algorithm/math/number_theory/gaussian/two_square_representations.hpp"
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int q; std::cin >> q;
    while(q--){
        std::uint64_t n; std::cin >> n;
        const auto result=math::two_square_representations(n);
        std::cout << result.size() << '\n';
        for(auto [a,b]:result) std::cout << a << ' ' << b << '\n';
    }
}
