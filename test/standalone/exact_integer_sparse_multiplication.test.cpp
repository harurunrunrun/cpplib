// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>

#include "../../src/algorithm/math/integer/exact_integer.hpp"

int main(){
    constexpr std::size_t left_limb = 800'000;
    constexpr std::size_t right_limb = 480'000;
    constexpr std::size_t limb_bits = 32;

    ExactInteger left = 1;
    ExactInteger right = 1;
    left <<= limb_bits * left_limb;
    right <<= limb_bits * right_limb;

    const ExactInteger product = left * right;
    ExactInteger expected = 1;
    expected <<= limb_bits * (left_limb + right_limb);
    assert(product == expected);
    std::cout << "OK\n";
}
