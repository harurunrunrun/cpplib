#ifndef CPPLIB_SRC_ALGORITHM_MATH_BASIC_NUMBER_THEORY_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_BASIC_NUMBER_THEORY_HPP_INCLUDED

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <vector>

namespace math{

namespace basic_number_theory_internal{

constexpr bool square_less_equal(long long value, long long limit){
    return value >= 0 && limit >= 0
        && (value == 0 || value <= limit / value);
}

} // namespace basic_number_theory_internal

struct ExtGcdResult{
    long long g;
    long long x;
    long long y;
};

struct QuotientRange{
    long long l;
    long long r;
    long long value;
};

constexpr long long abs_ll(long long x){
    return x < 0 ? -x : x;
}

constexpr long long gcd(long long a, long long b){
    a = abs_ll(a);
    b = abs_ll(b);
    while(b != 0){
        const long long c = a % b;
        a = b;
        b = c;
    }
    return a;
}

constexpr long long lcm(long long a, long long b){
    if(a == 0 || b == 0){
        return 0;
    }
    return abs_ll(a / gcd(a, b) * b);
}

constexpr ExtGcdResult ext_gcd(long long a, long long b){
    const long long sa = a < 0 ? -1 : 1;
    const long long sb = b < 0 ? -1 : 1;
    a = abs_ll(a);
    b = abs_ll(b);
    if(a == 0 && b == 0){
        return {0, 0, 0};
    }

    long long x0 = 1, y0 = 0;
    long long x1 = 0, y1 = 1;
    while(b != 0){
        const long long q = a / b;
        const long long na = a - q * b;
        const long long nx = x0 - q * x1;
        const long long ny = y0 - q * y1;
        a = b;
        b = na;
        x0 = x1;
        x1 = nx;
        y0 = y1;
        y1 = ny;
    }
    return {a, x0 * sa, y0 * sb};
}

long long pow_mod(long long x, long long n, long long mod){
    if(n < 0 || mod <= 0)[[unlikely]]{
        throw std::runtime_error("library assertion fault: invalid argument (pow_mod).");
    }
    if(mod == 1){
        return 0;
    }
    x %= mod;
    if(x < 0){
        x += mod;
    }
    long long res = 1 % mod;
    while(n > 0){
        if(n & 1){
            res = (long long)((__int128)res * x % mod);
        }
        x = (long long)((__int128)x * x % mod);
        n >>= 1;
    }
    return res;
}

long long inv_mod(long long a, long long mod){
    if(mod <= 0)[[unlikely]]{
        throw std::runtime_error("library assertion fault: invalid argument (inv_mod).");
    }
    const auto res = ext_gcd(a, mod);
    if(res.g != 1)[[unlikely]]{
        throw std::runtime_error("library assertion fault: inverse does not exist (inv_mod).");
    }
    long long x = res.x % mod;
    if(x < 0){
        x += mod;
    }
    return x;
}

long long inv_mod_prime(long long a, long long mod){
    if(mod <= 1 || a % mod == 0)[[unlikely]]{
        throw std::runtime_error("library assertion fault: invalid argument (inv_mod_prime).");
    }
    return pow_mod(a, mod - 2, mod);
}

long long isqrt(long long n){
    if(n < 0)[[unlikely]]{
        throw std::runtime_error("library assertion fault: invalid argument (isqrt).");
    }
    long long x = (long long)__builtin_sqrt((long double)n);
    while((__int128)(x + 1) * (x + 1) <= n){
        x++;
    }
    while((__int128)x * x > n){
        x--;
    }
    return x;
}

std::vector<long long> divisors(long long n){
    if(n == 0)[[unlikely]]{
        throw std::runtime_error("library assertion fault: invalid argument (divisors).");
    }
    n = abs_ll(n);
    std::vector<long long> small, large;
    for(long long d = 1;
        basic_number_theory_internal::square_less_equal(d, n);
        d++){
        if(n % d == 0){
            small.push_back(d);
            if(d * d != n){
                large.push_back(n / d);
            }
        }
    }
    small.insert(small.end(), large.rbegin(), large.rend());
    return small;
}

std::vector<QuotientRange> quotient_ranges(long long n){
    if(n < 0)[[unlikely]]{
        throw std::runtime_error("library assertion fault: invalid argument (quotient_ranges).");
    }
    std::vector<QuotientRange> res;
    for(long long l = 1; l <= n;){
        const long long q = n / l;
        const long long r = n / q + 1;
        res.push_back({l, r, q});
        l = r;
    }
    return res;
}

std::vector<int> to_base_digits(unsigned long long n, int base){
    if(base < 2)[[unlikely]]{
        throw std::runtime_error("library assertion fault: invalid argument (to_base_digits).");
    }
    if(n == 0){
        return {0};
    }
    std::vector<int> res;
    while(n > 0){
        res.push_back((int)(n % base));
        n /= base;
    }
    std::reverse(res.begin(), res.end());
    return res;
}

unsigned long long from_base_digits(const std::vector<int>& digits, int base){
    if(base < 2)[[unlikely]]{
        throw std::runtime_error("library assertion fault: invalid argument (from_base_digits).");
    }
    unsigned __int128 res = 0;
    for(int d: digits){
        if(d < 0 || base <= d)[[unlikely]]{
            throw std::runtime_error("library assertion fault: invalid digit (from_base_digits).");
        }
        res = res * base + d;
        if(res > std::numeric_limits<unsigned long long>::max())[[unlikely]]{
            throw std::runtime_error("library assertion fault: overflow (from_base_digits).");
        }
    }
    return (unsigned long long)res;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_BASIC_NUMBER_THEORY_HPP_INCLUDED
