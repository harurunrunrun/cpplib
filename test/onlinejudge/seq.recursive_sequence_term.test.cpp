// competitive-verifier: PROBLEM https://www.spoj.com/problems/SEQ/
#include <cstdint>
#include <iostream>
#include <vector>

#include "../../src/algorithm/math/recursive_sequence_term.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int tests;
    std::cin >> tests;
    while(tests--){
        int degree;
        std::cin >> degree;
        std::vector<std::uint64_t> initial(static_cast<std::size_t>(degree));
        std::vector<std::uint64_t> coefficient(static_cast<std::size_t>(degree));
        for(auto& value: initial) std::cin >> value;
        for(auto& value: coefficient) std::cin >> value;
        std::uint64_t index;
        std::cin >> index;
        std::cout << math::recursive_sequence_term(initial, coefficient, index) << '\n';
    }
}
