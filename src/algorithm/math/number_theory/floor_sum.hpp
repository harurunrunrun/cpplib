#ifndef CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_FLOOR_SUM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_FLOOR_SUM_HPP_INCLUDED

#include <limits>
#include <stdexcept>

namespace math{

namespace floor_sum_internal{

using i128 = __int128;
using u128 = unsigned __int128;

inline i128 checked_mul(i128 lhs, i128 rhs){
    if(lhs == 0 || rhs == 0) return 0;
    constexpr u128 positive_limit = static_cast<u128>(-1) >> 1;
    const bool negative = (lhs < 0) != (rhs < 0);
    const u128 lhs_abs = lhs < 0 ? static_cast<u128>(-(lhs + 1)) + 1 : static_cast<u128>(lhs);
    const u128 rhs_abs = rhs < 0 ? static_cast<u128>(-(rhs + 1)) + 1 : static_cast<u128>(rhs);
    const u128 limit = positive_limit + static_cast<u128>(negative);
    if(lhs_abs > limit / rhs_abs)[[unlikely]]{
        throw std::overflow_error("library assertion fault: overflow (floor_sum).");
    }
    const u128 product = lhs_abs * rhs_abs;
    if(!negative) return static_cast<i128>(product);
    if(product == positive_limit + 1) return -static_cast<i128>(positive_limit) - 1;
    return -static_cast<i128>(product);
}

inline i128 checked_add(i128 lhs, i128 rhs){
    constexpr i128 maximum = static_cast<i128>(static_cast<u128>(-1) >> 1);
    constexpr i128 minimum = -maximum - 1;
    if((rhs > 0 && lhs > maximum - rhs) || (rhs < 0 && lhs < minimum - rhs))[[unlikely]]{
        throw std::overflow_error("library assertion fault: overflow (floor_sum).");
    }
    return lhs + rhs;
}

inline u128 floor_sum_unsigned(u128 n, u128 m, u128 a, u128 b){
    if(n == 0) return 0;
    u128 answer = 0;
    while(true){
        if(a >= m){
            answer += n * (n - 1) / 2 * (a / m);
            a %= m;
        }
        if(b >= m){
            answer += n * (b / m);
            b %= m;
        }

        const u128 y_max = a * n + b;
        if(y_max < m) break;
        n = y_max / m;
        b = y_max % m;
        const u128 tmp = m;
        m = a;
        a = tmp;
    }
    return answer;
}

} // namespace floor_sum_internal

inline long long floor_sum(long long n, long long m, long long a, long long b){
    if(n < 0 || m <= 0)[[unlikely]]{
        throw std::runtime_error("library assertion fault: invalid argument (floor_sum).");
    }

    using floor_sum_internal::i128;
    using floor_sum_internal::u128;
    const i128 modulus = static_cast<i128>(m);
    const auto floor_div = [modulus](long long value){
        i128 quotient = static_cast<i128>(value) / modulus;
        const i128 remainder = static_cast<i128>(value) % modulus;
        if(remainder < 0) --quotient;
        return quotient;
    };

    const i128 a_quotient = floor_div(a);
    const i128 b_quotient = floor_div(b);
    const i128 a_remainder = static_cast<i128>(a) - a_quotient * modulus;
    const i128 b_remainder = static_cast<i128>(b) - b_quotient * modulus;
    const i128 count = static_cast<i128>(n);

    i128 answer = floor_sum_internal::checked_mul(
        floor_sum_internal::checked_mul(count, count - 1) / 2,
        a_quotient
    );
    answer = floor_sum_internal::checked_add(
        answer,
        floor_sum_internal::checked_mul(count, b_quotient)
    );
    const u128 unsigned_part = floor_sum_internal::floor_sum_unsigned(
        static_cast<u128>(n),
        static_cast<u128>(m),
        static_cast<u128>(a_remainder),
        static_cast<u128>(b_remainder)
    );
    constexpr u128 i128_maximum = static_cast<u128>(-1) >> 1;
    if(unsigned_part > i128_maximum)[[unlikely]]{
        throw std::overflow_error("library assertion fault: overflow (floor_sum).");
    }
    answer = floor_sum_internal::checked_add(answer, static_cast<i128>(unsigned_part));

    if(answer < std::numeric_limits<long long>::min() ||
       answer > std::numeric_limits<long long>::max())[[unlikely]]{
        throw std::overflow_error("library assertion fault: overflow (floor_sum).");
    }
    return static_cast<long long>(answer);
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_FLOOR_SUM_HPP_INCLUDED
