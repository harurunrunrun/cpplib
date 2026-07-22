#ifndef CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_GAUSSIAN_INTEGER_FACTORIZATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_GAUSSIAN_INTEGER_FACTORIZATION_HPP_INCLUDED

#include <algorithm>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "gaussian_integer.hpp"
#include "prime_factorization.hpp"
#include "sqrt_mod.hpp"

namespace math{

struct GaussianPrimePower{
    GaussianInteger prime{};
    int exponent = 0;

    friend bool operator==(const GaussianPrimePower&, const GaussianPrimePower&) = default;
};

struct GaussianIntegerFactorization{
    GaussianInteger unit{1, 0};
    std::vector<GaussianPrimePower> factors;

    friend bool operator==(
        const GaussianIntegerFactorization&,
        const GaussianIntegerFactorization&
    ) = default;
};

namespace gaussian_integer_factorization_internal{

using i128 = __int128_t;
using u128 = __uint128_t;

inline u128 component_square(long long value){
    const i128 extended = value;
    const u128 magnitude = extended < 0
        ? static_cast<u128>(-extended)
        : static_cast<u128>(extended);
    return magnitude * magnitude;
}

inline std::uint64_t norm_u64(GaussianInteger value){
    const u128 result = component_square(value.real) + component_square(value.imag);
    if(result > std::numeric_limits<std::uint64_t>::max())[[unlikely]]{
        throw std::overflow_error("Gaussian integer norm does not fit in uint64_t");
    }
    return static_cast<std::uint64_t>(result);
}

inline std::uint64_t floor_square_root(std::uint64_t value){
    std::uint64_t lower = 0;
    std::uint64_t upper = std::uint64_t{1} << 32U;
    while(lower + 1 < upper){
        const std::uint64_t middle = lower + (upper - lower) / 2;
        if(static_cast<u128>(middle) * middle <= value){
            lower = middle;
        }else{
            upper = middle;
        }
    }
    return lower;
}

inline GaussianInteger split_rational_prime(std::uint64_t prime){
    const auto root = sqrt_mod(prime - 1, prime);
    if(!root.has_value())[[unlikely]]{
        throw std::logic_error("library assertion fault: -1 has no square root modulo split prime");
    }

    std::uint64_t previous = prime;
    std::uint64_t current = *root;
    while(static_cast<u128>(current) * current > prime){
        const std::uint64_t next = previous % current;
        previous = current;
        current = next;
    }
    const std::uint64_t other_square = prime - current * current;
    const std::uint64_t other = floor_square_root(other_square);
    if(other * other != other_square)[[unlikely]]{
        throw std::logic_error("library assertion fault: Cornacchia reconstruction failed");
    }
    return gaussian_integer_internal::canonical_associate({
        static_cast<long long>(current),
        static_cast<long long>(other)
    });
}

inline bool divide_if_exact(GaussianInteger& value, GaussianInteger divisor){
    const i128 denominator = static_cast<i128>(divisor.real) * divisor.real
        + static_cast<i128>(divisor.imag) * divisor.imag;
    const i128 real_numerator = static_cast<i128>(value.real) * divisor.real
        + static_cast<i128>(value.imag) * divisor.imag;
    const i128 imag_numerator = static_cast<i128>(value.imag) * divisor.real
        - static_cast<i128>(value.real) * divisor.imag;
    if(real_numerator % denominator != 0 || imag_numerator % denominator != 0){
        return false;
    }
    value = {
        gaussian_integer_internal::narrow(real_numerator / denominator),
        gaussian_integer_internal::narrow(imag_numerator / denominator)
    };
    return true;
}

inline void append_factor(
    std::vector<GaussianPrimePower>& factors,
    GaussianInteger prime,
    int exponent
){
    if(exponent != 0) factors.push_back({prime, exponent});
}

} // namespace gaussian_integer_factorization_internal

inline GaussianIntegerFactorization factorize_gaussian_integer(GaussianInteger value){
    using namespace gaussian_integer_factorization_internal;

    const std::uint64_t value_norm = norm_u64(value);
    if(value_norm == 0)[[unlikely]]{
        throw std::invalid_argument("factorization of the zero Gaussian integer is undefined");
    }

    GaussianIntegerFactorization result;
    GaussianInteger remaining = value;
    for(const auto [prime, norm_exponent]: factorize_pollard_rho(value_norm)){
        if(prime == 2){
            const GaussianInteger gaussian_prime{1, 1};
            for(int count = 0; count < norm_exponent; ++count){
                if(!divide_if_exact(remaining, gaussian_prime))[[unlikely]]{
                    throw std::logic_error("library assertion fault: missing ramified Gaussian factor");
                }
            }
            append_factor(result.factors, gaussian_prime, norm_exponent);
            continue;
        }

        if(prime % 4 == 3){
            if((norm_exponent & 1) != 0)[[unlikely]]{
                throw std::logic_error("library assertion fault: odd inert-prime norm valuation");
            }
            const GaussianInteger gaussian_prime{static_cast<long long>(prime), 0};
            const int exponent = norm_exponent / 2;
            for(int count = 0; count < exponent; ++count){
                if(!divide_if_exact(remaining, gaussian_prime))[[unlikely]]{
                    throw std::logic_error("library assertion fault: missing inert Gaussian factor");
                }
            }
            append_factor(result.factors, gaussian_prime, exponent);
            continue;
        }

        const GaussianInteger first = split_rational_prime(prime);
        const GaussianInteger second = gaussian_integer_internal::canonical_associate({
            first.real, -first.imag
        });
        int first_exponent = 0;
        while(first_exponent < norm_exponent && divide_if_exact(remaining, first)){
            ++first_exponent;
        }
        int second_exponent = 0;
        while(first_exponent + second_exponent < norm_exponent &&
              divide_if_exact(remaining, second)){
            ++second_exponent;
        }
        if(first_exponent + second_exponent != norm_exponent)[[unlikely]]{
            throw std::logic_error("library assertion fault: missing split Gaussian factor");
        }
        append_factor(result.factors, first, first_exponent);
        append_factor(result.factors, second, second_exponent);
    }

    if(norm_u64(remaining) != 1)[[unlikely]]{
        throw std::logic_error("library assertion fault: Gaussian factorization left a non-unit");
    }
    result.unit = remaining;
    std::sort(result.factors.begin(), result.factors.end(), [](const auto& left, const auto& right){
        return std::pair{left.prime.real, left.prime.imag}
            < std::pair{right.prime.real, right.prime.imag};
    });
    return result;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_GAUSSIAN_INTEGER_FACTORIZATION_HPP_INCLUDED
