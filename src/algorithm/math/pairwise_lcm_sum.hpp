#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace math{

namespace pairwise_lcm_sum_internal{

inline std::int64_t power_mod(
    std::int64_t base,
    std::int64_t exponent,
    std::int64_t modulus
){
    std::int64_t result = 1;
    while(exponent != 0){
        if(exponent & 1){
            result = static_cast<std::int64_t>(
                static_cast<__int128>(result) * base % modulus
            );
        }
        exponent >>= 1;
        if(exponent != 0){
            base = static_cast<std::int64_t>(
                static_cast<__int128>(base) * base % modulus
            );
        }
    }
    return result;
}

} // namespace pairwise_lcm_sum_internal

inline std::int64_t pairwise_lcm_sum(
    const std::vector<int>& values
){
    constexpr std::int64_t modulus = 998'244'353;
    int limit = 0;
    for(const int value: values){
        if(value <= 0)[[unlikely]]{
            throw std::invalid_argument("pairwise_lcm_sum requires positive values");
        }
        if(value % modulus == 0)[[unlikely]]{
            throw std::invalid_argument(
                "pairwise_lcm_sum requires values coprime to 998244353");
        }
        limit = std::max(limit, value);
    }
    std::vector<std::int64_t> frequency(static_cast<std::size_t>(limit) + 1);
    for(const int value: values) ++frequency[static_cast<std::size_t>(value)];
    std::vector<std::int64_t> pair_product(static_cast<std::size_t>(limit) + 1);
    for(int divisor = 1; divisor <= limit; ++divisor){
        std::int64_t sum = 0;
        std::int64_t square_sum = 0;
        for(int multiple = divisor; multiple <= limit; multiple += divisor){
            const std::int64_t count = frequency[static_cast<std::size_t>(multiple)];
            sum = (sum + count * multiple) % modulus;
            square_sum = static_cast<std::int64_t>(
                (square_sum + static_cast<__int128>(count) * multiple % modulus *
                    multiple) % modulus
            );
        }
        const std::int64_t numerator =
            (static_cast<std::int64_t>(static_cast<__int128>(sum) * sum % modulus) -
                square_sum + modulus) % modulus;
        pair_product[static_cast<std::size_t>(divisor)] =
            static_cast<std::int64_t>(
                static_cast<__int128>(numerator) * ((modulus + 1) / 2) % modulus
            );
    }
    std::int64_t answer = 0;
    for(int gcd = limit; gcd >= 1; --gcd){
        auto& exact = pair_product[static_cast<std::size_t>(gcd)];
        for(int multiple = gcd + gcd; multiple <= limit; multiple += gcd){
            exact -= pair_product[static_cast<std::size_t>(multiple)];
            if(exact < 0) exact += modulus;
        }
        const std::int64_t inverse = pairwise_lcm_sum_internal::power_mod(
            gcd % modulus, modulus - 2, modulus
        );
        answer = static_cast<std::int64_t>(
            (answer + static_cast<__int128>(exact) * inverse) % modulus
        );
    }
    return answer;
}

} // namespace math
