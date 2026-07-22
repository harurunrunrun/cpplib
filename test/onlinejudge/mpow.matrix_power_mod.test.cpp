// competitive-verifier: PROBLEM https://www.spoj.com/problems/MPOW/
#include <cstdint>
#include <iostream>

#include "../../src/algorithm/math/linear_algebra/matrix_power_mod.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int tests;
    std::cin >> tests;
    while(tests--){
        int size;
        std::uint64_t exponent;
        std::cin >> size >> exponent;
        math::DynamicModMatrix matrix(
            static_cast<std::size_t>(size),
            std::vector<std::int64_t>(static_cast<std::size_t>(size))
        );
        for(auto& row: matrix) for(auto& value: row) std::cin >> value;
        const auto result = math::matrix_power_mod(std::move(matrix), exponent);
        for(const auto& row: result){
            for(int column = 0; column < size; ++column){
                if(column != 0) std::cout << ' ';
                std::cout << row[static_cast<std::size_t>(column)];
            }
            std::cout << '\n';
        }
    }
}
