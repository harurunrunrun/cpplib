// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/polynomial_taylor_shift

#include <cstddef>
#include <iostream>

#include "../../src/algorithm/math/polynomial/polynomial_taylor_shift.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::size_t size;
    long long shift;
    std::cin >> size >> shift;
    math::Polynomial<998244353> polynomial(size);
    for(auto& coefficient: polynomial) std::cin >> coefficient;
    const auto answer = math::polynomial_taylor_shift<998244353>(
        polynomial, Modint<998244353>(shift)
    );
    for(std::size_t index = 0; index < answer.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << answer[index];
    }
    std::cout << '\n';
}
