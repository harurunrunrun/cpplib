// competitive-verifier: STANDALONE

#include <cassert>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/math/basic_number_theory.hpp"

void test_base_digits(){
    assert((math::to_base_digits(0, 2) == std::vector<int>{0}));
    assert((math::to_base_digits(42, 2) == std::vector<int>{1, 0, 1, 0, 1, 0}));
    assert((math::to_base_digits(255, 16) == std::vector<int>{15, 15}));
    assert(math::from_base_digits({1, 0, 1, 0, 1, 0}, 2) == 42);
    assert(math::from_base_digits({15, 15}, 16) == 255);
}

void test_divisors_and_quotients(){
    assert((math::divisors(36) == std::vector<long long>{1, 2, 3, 4, 6, 9, 12, 18, 36}));
    assert((math::divisors(-10) == std::vector<long long>{1, 2, 5, 10}));

    const auto ranges = math::quotient_ranges(10);
    assert(ranges.size() == 5);
    assert(ranges[0].l == 1 && ranges[0].r == 2 && ranges[0].value == 10);
    assert(ranges[1].l == 2 && ranges[1].r == 3 && ranges[1].value == 5);
    assert(ranges[2].l == 3 && ranges[2].r == 4 && ranges[2].value == 3);
    assert(ranges[3].l == 4 && ranges[3].r == 6 && ranges[3].value == 2);
    assert(ranges[4].l == 6 && ranges[4].r == 11 && ranges[4].value == 1);
}

void test_gcd_lcm_extgcd(){
    static_assert(math::gcd(18, 24) == 6);
    static_assert(math::gcd(-18, 24) == 6);
    static_assert(math::lcm(6, 10) == 30);
    static_assert(math::lcm(0, 10) == 0);

    const auto r = math::ext_gcd(30, 18);
    assert(r.g == 6);
    assert(30 * r.x + 18 * r.y == r.g);

    const auto neg = math::ext_gcd(-30, 18);
    assert(neg.g == 6);
    assert(-30 * neg.x + 18 * neg.y == neg.g);
}

void test_modular(){
    assert(math::pow_mod(2, 10, 1000) == 24);
    assert(math::pow_mod(-2, 5, 13) == 7);
    assert(math::inv_mod(3, 11) == 4);
    assert(math::inv_mod(10, 17) == 12);
    assert(math::inv_mod_prime(3, 11) == 4);
}

void test_isqrt(){
    assert(math::isqrt(0) == 0);
    assert(math::isqrt(1) == 1);
    assert(math::isqrt(15) == 3);
    assert(math::isqrt(16) == 4);
    assert(math::isqrt(17) == 4);
    assert(math::isqrt(999999999999999999LL) == 999999999);
}

void test_exceptions(){
    bool thrown = false;
    try{
        (void)math::divisors(0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)math::inv_mod(6, 9);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)math::from_base_digits({1, 2}, 2);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    test_base_digits();
    test_divisors_and_quotients();
    test_gcd_lcm_extgcd();
    test_modular();
    test_isqrt();
    test_exceptions();
}
