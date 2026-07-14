// competitive-verifier: PROBLEM https://www.spoj.com/problems/SQFREE/
#include <cstdint>
#include <iostream>

#include "../../src/algorithm/math/squarefree_integer_count.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int tests;
    std::cin >> tests;
    while(tests--){
        std::uint64_t n;
        std::cin >> n;
        std::cout << math::squarefree_integer_count(n) << '\n';
    }
}
