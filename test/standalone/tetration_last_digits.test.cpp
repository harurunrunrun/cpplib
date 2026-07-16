// competitive-verifier: STANDALONE

#include <cstdint>
#include <iostream>

#include "../../src/algorithm/math/power_tower_last_digits.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int query_count;
    std::cin >> query_count;
    while(query_count--){
        std::uint64_t base;
        std::uint64_t height;
        unsigned digit_count;
        std::cin >> base >> height >> digit_count;
        const auto result =
            math::tetration_last_digits(base, height, digit_count);
        std::cout << result.value << ' ' << result.digit_count << ' '
                  << result.truncated << ' ' << result.to_string() << '\n';
    }
}
