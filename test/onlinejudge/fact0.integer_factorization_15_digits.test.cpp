// competitive-verifier: PROBLEM https://www.spoj.com/problems/FACT0/
#include <cstdint>
#include <iostream>

#include "../../src/algorithm/math/integer_factorization_15_digits.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    for(std::uint64_t value; std::cin >> value && value != 0;){
        const auto factors = math::integer_factorization_15_digits(value);
        for(std::size_t i = 0; i < factors.size(); ++i){
            if(i != 0) std::cout << ' ';
            std::cout << factors[i].first << '^' << factors[i].second;
        }
        std::cout << '\n';
    }
}
