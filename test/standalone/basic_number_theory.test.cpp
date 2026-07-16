// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
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

    constexpr long long maximum = 9'223'372'036'854'775'807LL;
    static_assert(math::basic_number_theory_internal::square_less_equal(
        3'037'000'499LL, maximum));
    static_assert(!math::basic_number_theory_internal::square_less_equal(
        3'037'000'500LL, maximum));
}

void test_gcd_lcm_extgcd(){
    static_assert(math::abs_ll(0) == 0);
    static_assert(math::abs_ll(42) == 42);
    static_assert(math::abs_ll(-42) == 42);

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
    int q;
    if(std::cin >> q){
        while(q--){
            std::string type;
            std::cin >> type;
            if(type == "SQLE"){
                long long value, limit;
                std::cin >> value >> limit;
                std::cout
                    << math::basic_number_theory_internal::square_less_equal(
                        value, limit)
                    << '\n';
            }else if(type == "GCD" || type == "LCM" || type == "EXT"){
                long long a, b;
                std::cin >> a >> b;
                if(type == "GCD") std::cout << math::gcd(a, b) << '\n';
                if(type == "LCM") std::cout << math::lcm(a, b) << '\n';
                if(type == "EXT"){
                    const auto res = math::ext_gcd(a, b);
                    std::cout << res.g << ' ' << a * res.x + b * res.y << '\n';
                }
            }else if(type == "POW"){
                long long x, n, mod;
                std::cin >> x >> n >> mod;
                std::cout << math::pow_mod(x, n, mod) << '\n';
            }else if(type == "INV"){
                long long a, mod;
                std::cin >> a >> mod;
                std::cout << math::inv_mod(a, mod) << '\n';
            }else if(type == "ISQRT"){
                long long n;
                std::cin >> n;
                std::cout << math::isqrt(n) << '\n';
            }else if(type == "DIVISORS"){
                long long n;
                std::cin >> n;
                const auto values = math::divisors(n);
                std::cout << values.size();
                for(long long value: values) std::cout << ' ' << value;
                std::cout << '\n';
            }else if(type == "QUOTIENT"){
                long long n;
                std::cin >> n;
                const auto ranges = math::quotient_ranges(n);
                std::cout << ranges.size();
                for(const auto& range: ranges){
                    std::cout << ' ' << range.l << ' ' << range.r << ' ' << range.value;
                }
                std::cout << '\n';
            }else if(type == "TODIG"){
                unsigned long long n;
                int base;
                std::cin >> n >> base;
                const auto digits = math::to_base_digits(n, base);
                std::cout << digits.size();
                for(int digit: digits) std::cout << ' ' << digit;
                std::cout << '\n';
            }else if(type == "FROM"){
                int base, count;
                std::cin >> base >> count;
                std::vector<int> digits(static_cast<std::size_t>(count));
                for(int& digit: digits) std::cin >> digit;
                std::cout << math::from_base_digits(digits, base) << '\n';
            }
        }
        return 0;
    }

    test_base_digits();
    test_divisors_and_quotients();
    test_gcd_lcm_extgcd();
    test_modular();
    test_isqrt();
    test_exceptions();
}
