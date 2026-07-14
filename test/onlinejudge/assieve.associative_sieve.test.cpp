// competitive-verifier: PROBLEM https://www.spoj.com/problems/ASSIEVE/
#include <cstddef>
#include <cstdint>
#include <iostream>

#include "../../src/algorithm/math/associative_sieve.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::uint64_t n;
    std::uint32_t exponent;
    std::cin >> n >> exponent;
    math::AssociativeSieveOperation operation{};
    for(std::size_t left = 0; left < 4; ++left){
        for(std::size_t right = 0; right < 4; ++right){
            unsigned value;
            std::cin >> value;
            operation[left][right] = static_cast<std::uint8_t>(value);
        }
    }
    const auto answers = math::associative_sieve_summatory_quotients(
        n, exponent, operation
    );
    for(const auto& answer: answers){
        std::cout << answer[0] << ' ' << answer[1] << ' '
                  << answer[2] << ' ' << answer[3] << '\n';
    }
}
