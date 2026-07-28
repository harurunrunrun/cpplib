// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <random>
#include <vector>

#include "../../src/algorithm/math/number_theory/multiplicative/dirichlet_convolution.hpp"

int main() {
    unsigned long long seed;
    int rounds;
    if(!(std::cin >> seed >> rounds)) return 0;
    std::mt19937_64 random(seed);
    for(int round = 0; round < rounds; ++round){
        const std::size_t maximum = 1 + random() % 200;
        std::vector<long double> first(maximum + 1);
        std::vector<long double> second(maximum + 1);
        for(std::size_t index = 1; index <= maximum; ++index){
            first[index] = static_cast<long double>(
                static_cast<long long>(random() % 11) - 5
            );
            second[index] = static_cast<long double>(
                static_cast<long long>(random() % 11) - 5
            );
        }
        const auto result = math::dirichlet_convolution(first, second);
        for(std::size_t value = 1; value <= maximum; ++value){
            long double expected = 0;
            for(std::size_t divisor = 1; divisor <= value; ++divisor){
                if(value % divisor == 0){
                    expected += first[divisor] * second[value / divisor];
                }
            }
            assert(result[value] == expected);
        }

        first[1] = 1;
        const auto inverse = math::dirichlet_inverse(first);
        const auto identity = math::dirichlet_convolution(first, inverse);
        assert(identity[1] == 1);
        for(std::size_t value = 2; value <= maximum; ++value){
            assert(identity[value] == 0);
        }
    }
    std::cout << "OK\n";
}
