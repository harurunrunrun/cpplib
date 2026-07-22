// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>

#include "../../src/algorithm/math/number_theory/binary_gcd.hpp"

void test_binary_gcd(){
    using u64 = std::uint64_t;
    constexpr u64 maximum = std::numeric_limits<u64>::max();
    static_assert(math::binary_gcd(0, 0) == 0);
    static_assert(math::binary_gcd(0, maximum) == maximum);
    static_assert(math::binary_gcd(maximum, maximum) == maximum);
    static_assert(math::binary_gcd(48, 18) == 6);
    static_assert(math::binary_gcd(1ULL << 63, 1ULL << 62) == (1ULL << 62));
    static_assert(math::binary_gcd(maximum, maximum - 2) == 1);
}

void test_signed_binary_gcd(){
    using i64 = std::int64_t;
    using u64 = std::uint64_t;
    constexpr i64 minimum = std::numeric_limits<i64>::min();
    static_assert(math::binary_gcd_signed(-48, 18) == 6);
    static_assert(math::binary_gcd_signed(minimum, 0) == (u64{1} << 63));
    static_assert(math::binary_gcd_signed(minimum, minimum) == (u64{1} << 63));
    static_assert(math::binary_gcd_signed(minimum, 1) == 1);
}

int main(){
    int query_count;
    if(std::cin >> query_count){
        while(query_count--){
            std::string type;
            std::cin >> type;
            if(type == "U"){
                std::uint64_t first, second;
                std::cin >> first >> second;
                std::cout << math::binary_gcd(first, second) << '\n';
            }else{
                std::int64_t first, second;
                std::cin >> first >> second;
                std::cout << math::binary_gcd_signed(first, second) << '\n';
            }
        }
        return 0;
    }
    test_binary_gcd();
    test_signed_binary_gcd();
}
