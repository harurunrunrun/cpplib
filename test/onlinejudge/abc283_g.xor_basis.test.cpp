// competitive-verifier: PROBLEM https://atcoder.jp/contests/abc283/tasks/abc283_g

#include <cstdint>
#include <iostream>

#include "../../src/structure/other/xor_basis.hpp"

int main(){
    int n;
    unsigned long long left, right;
    std::cin >> n >> left >> right;
    XorBasis<std::uint64_t, 60> basis;
    for(int i = 0; i < n; ++i){
        std::uint64_t value;
        std::cin >> value;
        basis.insert(value);
    }
    for(unsigned long long k = left; k <= right; ++k){
        if(k != left) std::cout << ' ';
        std::cout << basis.kth_smallest(k - 1);
    }
    std::cout << '\n';
}
